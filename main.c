#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"
#include "kheap.h"
#include "paging.h"

extern u32int end;
extern heap_t *kheap;

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

	monitor_write("kernel end at ");
	monitor_write_hex((u32int)&end);
	monitor_put('\n');

	u32int addr;
	print_placement_address();

	addr = kmalloc(8);
	monitor_write("kmalloc(8) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_placement_address();

	addr = kmalloc_a(0x1000);
	monitor_write("kmalloc_a(0x1000) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_placement_address();

	addr = kmalloc_a(8);
	monitor_write("kmalloc_a(8) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_placement_address();

	init_paging();
	monitor_write("Hello, paging world!\n");
	print_heap(kheap);
	break_point();

	u32int *a = (u32int *)kmalloc(8);
	u32int *b = (u32int *)kmalloc(8);
	u32int *c = (u32int *)kmalloc(8);
	u32int *d = (u32int *)kmalloc(8);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	kfree(d);
	print_heap(kheap);
	print_holes(kheap);
	break_point();
	kfree(b);
	print_heap(kheap);
	print_holes(kheap);
	break_point();
	kfree(c);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	u32int *e = alloc(12, 0, kheap);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	u32int *f = alloc(512, 1, kheap);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	u32int *g = (u32int *)kmalloc(200);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	return 0;
}
