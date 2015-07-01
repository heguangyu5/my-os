#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"
#include "kheap.h"
#include "paging.h"

int main(void *multibootPtr)
{
	init_gdt();
	init_idt();

	monitor_clear();
	monitor_write("Hello, World!\n");

	asm volatile("int $0x3");
	asm volatile("int $0x4");

	//asm volatile("sti");
	//init_timer(50); // 50Hz 

	u32int addr;
	print_kheap_brk();

	addr = kmalloc(8);
	monitor_write("kmalloc(8) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_kheap_brk();

	addr = kmalloc_a(0x1000);
	monitor_write("kmalloc_a(0x1000) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_kheap_brk();

	addr = kmalloc_a(8);
	monitor_write("kmalloc_a(8) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_kheap_brk();

	init_paging();
	monitor_write("Hello, paging world!\n");

	u32int *ptr = (u32int *)0xA0000000;
	u32int do_page_fault = *ptr;

	return 0;
}
