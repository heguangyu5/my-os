#ifndef SYSCALL_H
#define SYSCALL_H

#include "common.h"

void init_syscalls();
void syscall_monitor_write(char *s);
void syscall_monitor_write_hex(u32int n);
void syscall_monitor_write_dec(u32int n);

#endif
