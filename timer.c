#include "timer.h"
#include "isr.h"
#include "monitor.h"

u32int tick = 0;

static void timer_callback(registers_t regs)
{
	tick++;
	monitor_write("Tick: ");
	monitor_write_dec(tick);
	monitor_put('\n');
}

void init_timer(u32int frequency)
{
	register_interrupt_handler(IRQ0, &timer_callback);

	u32int divisor = 1193180 / frequency;

	outb(0x43, 0x36);
	
	u8int l = (u8int)(divisor & 0xFF);
	u8int h = (u8int)((divisor >> 8) & 0xFF);

	outb(0x40, l);
	outb(0x40, h);
}
