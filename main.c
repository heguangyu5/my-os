#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"

int main(void *ptr)
{
	init_gdt();
	init_idt();

	monitor_clear();
	monitor_write("Hello, World!\n");

	asm volatile("int $0x3");
	asm volatile("int $0x4");

	asm volatile("sti");
	init_timer(50); // 50Hz 

	return 0;
}
