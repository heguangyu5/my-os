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

monitor_write("move stack to 0xE0000000\n");

	move_stack((void *)0xE0000000, 0x2000);

print_heap(kheap);
break_point();
print_page_direcotry(current_directory, 1);
break_point();

monitor_write("\n\nkmalloc current_task\n");

    u32int phys;
	current_task = ready_queue = (task_t *)kmalloc_p(sizeof(task_t), &phys);

monitor_write("current_task = ready_queue = ");
monitor_write_hex((u32int)current_task);
monitor_write(", phys = ");
monitor_write_hex(phys);
monitor_put('\n');
print_heap(kheap);
break_point();

	current_task->id = next_pid;
	current_task->esp = 0;
	current_task->ebp = 0;
	current_task->eip = 0;
	current_task->page_directory = current_directory;
	current_task->next = 0;
	current_task->interrupt = 0;
	current_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
	next_pid++;

monitor_write("current_task init ok\n");
print_task(current_task);
break_point();

	asm volatile("sti");
}

int fork()
{
	asm volatile("cli");

	task_t *parent_task = (task_t *)current_task;

monitor_write("clone_directory\n");
	page_directory_t *dir = clone_directory(current_directory);
print_page_direcotry(dir, 1);
break_point();

monitor_write("create new_task\n");
	task_t *new_task = (task_t *)kmalloc(sizeof(task_t));
	new_task->id = next_pid;
	new_task->esp = 0;
	new_task->ebp = 0;
	new_task->eip = 0;
	new_task->page_directory = dir;
	new_task->next = 0;
	new_task->interrupt = 0;
	new_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
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

		monitor_write("new task ready:\n");
		print_task(new_task);
		break_point();

		asm volatile("sti");

		return new_task->id;
	}

	// child
	return 0;
}

void switch_task()
{
	if (!current_task) return;
	if (current_task->next == 0 && current_task == ready_queue) return;

	u32int esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	eip = read_eip();
	if (current_task->interrupt) {
	    current_task->interrupt = 0;
		return;
	}

//	monitor_write("\n\ncurrent task will be interrupt:\n");
//	print_task(current_task);
//	break_point();

	current_task->eip = eip;
	current_task->esp = esp;
	current_task->ebp = ebp;
	current_task->interrupt = 1;

//	monitor_write("current_task saved info:\n");
//	print_task(current_task);
//	break_point();

//    monitor_write("switch to next task\n");
	current_task = current_task->next;
	if (!current_task) current_task = ready_queue;

	eip = current_task->eip;
	esp = current_task->esp;
	ebp = current_task->ebp;

//	monitor_write("next_task info:\n");
//	print_task(current_task);
//	break_point();

	current_directory = current_task->page_directory;
	set_kernel_stack(current_task->kernel_stack + KERNEL_STACK_SIZE);

//	monitor_write("next_task begin");
//	break_point();
	do_switch_task(
	    current_task->eip,
	    current_task->esp,
	    current_task->ebp,
	    current_directory->physicalAddr,
	    current_task->interrupt
	);
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

void print_task(volatile task_t *task)
{
    monitor_write("task addr: ");
    monitor_write_hex((u32int)task);
    monitor_put('\n');

    monitor_write("id  = ");
    monitor_write_dec(task->id);
    monitor_put('\n');

    monitor_write("esp = ");
    monitor_write_hex(task->esp);
    monitor_put('(');
    monitor_write_hex(virt2phys(task->esp));
    monitor_write(")\n");

    monitor_write("ebp = ");
    monitor_write_hex(task->ebp);
    monitor_put('(');
    monitor_write_hex(virt2phys(task->ebp));
    monitor_write(")\n");

    monitor_write("eip = ");
    monitor_write_hex(task->eip);
    monitor_put('(');
    monitor_write_hex(virt2phys(task->eip));
    monitor_write(")\n");

    monitor_write("interrupt = ");
    monitor_write_dec(task->interrupt);
    monitor_put('\n');

    monitor_write("kernel_stack = ");
    monitor_write_hex(task->kernel_stack);
    monitor_put('\n');

    print_page_direcotry(task->page_directory, 0);

    monitor_write("next task: ");
    task_t *next_task = task->next;
    if (next_task) {
        monitor_write_dec(next_task->id);
    } else {
        monitor_write("none");
    }
    monitor_put('\n');
}

void switch_to_user_mode()
{
    set_kernel_stack(current_task->kernel_stack + KERNEL_STACK_SIZE);
    do_switch_to_user_mode();
}
