#include "multiboot.h"
#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"
#include "kheap.h"
#include "paging.h"
#include "fs.h"
#include "task.h"
#include "syscall.h"

extern u32int end;
extern u32int placement_address;
extern heap_t *kheap;
extern fs_node_t *initrd_root;
extern fs_node_t *initrd_dev;
extern fs_node_t *root_nodes;
extern isr_t interrupt_handlers[];

u32int initial_esp;

int main(struct multiboot *mboot_ptr, u32int initial_stack)
{
	initial_esp = initial_stack;

	init_gdt();
	init_idt();
	memset(&interrupt_handlers, 0, sizeof(isr_t)*256);

	monitor_clear();
	monitor_write("init gdt idt ok\n");
	break_point();

	monitor_write("init timer\n");
	asm volatile("sti");
	init_timer(50); // 50Hz
	break_point();

	monitor_write("kernel end at ");
	monitor_write_hex((u32int)&end);
	monitor_put('\n');
	monitor_write("main stack start ");
	monitor_write_hex(initial_esp);
	monitor_put('\n');
	break_point();

    ASSERT(mboot_ptr->mods_count > 0);
	u32int initrd_location = mboot_ptr->mods_addr[0];
	u32int initrd_end	   = mboot_ptr->mods_addr[1];
	placement_address = initrd_end;
	monitor_write("initrd: ");
	monitor_write_hex(initrd_location);
	monitor_write(" ~ ");
	monitor_write_hex(initrd_end);
	monitor_write(", size ");
	monitor_write_dec(initrd_end - initrd_location);
	monitor_put('\n');
	print_placement_address();
	break_point();

	init_paging();
	init_tasking();

	monitor_write("init initrd\n");
	init_initrd(initrd_location);
	monitor_write("initrd_root at ");
	monitor_write_hex((u32int)initrd_root);
	monitor_put('\n');
	monitor_write("initrd_dev at ");
	monitor_write_hex((u32int)initrd_dev);
	monitor_put('\n');
	monitor_write("root_nodes at ");
	monitor_write_hex((u32int)root_nodes);
	monitor_put('\n');
	print_heap(kheap);
	break_point();

	monitor_write("init syscalls\n");
	init_syscalls();
	break_point();

	switch_to_user_mode();
	syscall_monitor_write("Hello, user world!\n");
	syscall_monitor_write_hex(0x123456);
	syscall_monitor_write("\n");
	syscall_monitor_write_dec(123456);
	syscall_monitor_write("\n");

	return 0;
}
