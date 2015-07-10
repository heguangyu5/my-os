.global call_syscall
call_syscall:
	call *4(%esp)
	pop %eax # ret addr
	pop %ebx # syscall[regs.eax]
	pop %ebx # regs.ebx
	pop %ebx # regs.ecx
	pop %ebx # regs.edx
	pop %ebx # regs.esi
	pop %ebx # regs.edi
	pop %ebx # &regs.eax
	jmp *%eax

.global syscall_monitor_write
syscall_monitor_write:
	mov $0, %eax
	mov 4(%esp), %ebx
	int $0x80
	ret

.global syscall_monitor_write_hex
syscall_monitor_write_hex:
	mov $1, %eax
	mov 4(%esp), %ebx
	int $0x80
	ret

.global syscall_monitor_write_dec
syscall_monitor_write_dec:
	mov $2, %eax
	mov 4(%esp), %ebx
	int $0x80
	ret
