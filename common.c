#include "common.h"
#include "monitor.h"

void memcpy(void *dest, void *src, u32int len)
{
	u8int *sp = (u8int *)src;
	u8int *dp = (u8int *)dest;
	while (len) {
		*dp = *sp;
		dp++;
		sp++;
		len--;
	}
}

void memset(void *dest, u8int val, u32int len)
{
	u8int *dp = (u8int *)dest;
	while (len) {
		*dp = val;
		dp++;
		len--;
	}
}

void panic(char *msg, char *file, u32int line)
{
	asm volatile("cli");

	monitor_write("PANIC(");
	monitor_write(msg);
	monitor_write(") at ");
	monitor_write(file);
	monitor_put(':');
	monitor_write_dec(line);
	monitor_put('\n');

	for(;;);
}

void panic_assert(char *file, u32int line, char *desc)
{
	asm volatile("cli");

	monitor_write("ASSERTION-FAILED(");
	monitor_write(desc);
	monitor_write(") at ");
	monitor_write(file);
	monitor_put(':');
	monitor_write_dec(line);
	monitor_put('\n');

	for(;;);
}
