#include "task.h"

extern page_directory_t *current_directory;
extern u32int initial_esp;

volatile task_t *current_task;
volatile task_t *ready_queue;

u32int next_pid = 1;

extern heap_t *kheap;

void init_tasking()
{
	asm volatile("cli");

	move_stack((void *)0xE0000000, 0x2000);

monitor_write("kmalloc current_task\n\n");
	current_task = ready_queue = (task_t *)kmalloc(sizeof(task_t));
	current_task->id = next_pid;
	current_task->esp = 0;
	current_task->ebp = 0;
	current_task->eip = 0;
	current_task->page_directory = current_directory;
	current_task->next = 0;
	current_task->interrupt = 0;
	next_pid++;

	asm volatile("sti");
}

int fork()
{
	asm volatile("cli");

	task_t *parent_task = (task_t *)current_task;

	page_directory_t *dir = clone_directory(current_directory);

	task_t *new_task = (task_t *)kmalloc(sizeof(task_t));
	new_task->id = next_pid;
	new_task->esp = 0;
	new_task->ebp = 0;
	new_task->eip = 0;
	new_task->page_directory = dir;
	new_task->next = 0;
	new_task->interrupt = 0;
	next_pid++;

	task_t *tmp_task = (task_t *)ready_queue;
	while (tmp_task->next) {
		tmp_task = tmp_task->next;
	}
	tmp_task->next = new_task;

	u32int eip = read_eip();
	if (current_task == parent_task) {
		// parent
		u32int esp;
		u32int ebp;
		asm volatile("mov %%esp, %0": "=r"(esp));
		asm volatile("mov %%ebp, %0": "=r"(ebp));
		new_task->esp = esp;
		new_task->ebp = ebp;
		new_task->eip = eip;

		asm volatile("sti");

		return new_task->id;
	}

	// child
	return 0;
}

void switch_task()
{
	if (!current_task) return;

	u32int esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	eip = read_eip();
	if (current_task->interrupt) {
	    current_task->interrupt = 0;
		return;
	}
	current_task->eip = eip;
	current_task->esp = esp;
	current_task->ebp = ebp;
	current_task->interrupt = 1;

	current_task = current_task->next;
	if (!current_task) current_task = ready_queue;

    current_task->interrupt = 0;
	eip = current_task->eip;
	esp = current_task->esp;
	ebp = current_task->ebp;

	current_directory = current_task->page_directory;

	asm volatile("			\
		mov %0, %%ecx;		\
		mov %1, %%esp;		\
		mov %2, %%ebp;		\
		mov %3, %%cr3;		\
		sti;				\
		jmp *%%ecx"
					: : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));
}

void move_stack(void *new_stack_start, u32int size)
{
	u32int i;
	for (i = (u32int)new_stack_start; i >= (u32int)new_stack_start - size; i -= 0x1000) {
		alloc_frame(get_page(i, 1, current_directory), 0, 1);
	}

	u32int pd_addr;
	asm volatile("mov %%cr3, %0": "=r"(pd_addr));
	asm volatile("mov %0, %%cr3": : "r"(pd_addr));

	u32int old_stack_pointer;
	asm volatile("mov %%esp, %0": "=r"(old_stack_pointer));
	u32int old_base_pointer;
	asm volatile("mov %%ebp, %0": "=r"(old_base_pointer));

	u32int offset = (u32int)new_stack_start - initial_esp;

	u32int new_stack_pointer = old_stack_pointer + offset;
	u32int new_base_pointer = old_base_pointer + offset;

	memcpy((void *)new_stack_pointer, (void *)old_stack_pointer, initial_esp - old_stack_pointer);

	for (i = (u32int)new_stack_start; i > (u32int)new_stack_start - size; i -= 4) {
		u32int tmp = *(u32int *)i;
		if (tmp > old_stack_pointer && tmp < initial_esp) {
			tmp += offset;
			u32int *tmp2 = (u32int *)i;
			*tmp2 = tmp;
		}
	}

	asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
	asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}

int getpid()
{
	return current_task->id;
}
