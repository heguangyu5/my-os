#include "monitor.h"
#include "gdt-idt.h"

int main(void *ptr)
{
	init_gdt();
	init_idt();

	monitor_clear();
	monitor_write("Hello, World!\n");

	asm volatile("int $0x03");
	asm volatile("int $0x04");

	return 0;
}
