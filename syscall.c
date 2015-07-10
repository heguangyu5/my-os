#include "syscall.h"
#include "isr.h"
#include "monitor.h"

static void *syscall[3] = {
	&monitor_write,
	&monitor_write_hex,
	&monitor_write_dec
};
u32int num_syscalls = 3;

static void syscall_handler(registers_t regs)
{
	if (regs.eax >= num_syscalls) {
		return;
	}

	call_syscall(
		syscall[regs.eax],
		regs.ebx,
		regs.ecx,
		regs.edx,
		regs.esi,
		regs.edi
	);
}

void init_syscalls()
{
	register_interrupt_handler(0x80, &syscall_handler);
}
