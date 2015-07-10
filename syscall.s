.global call_syscall
call_syscall:
	push %ebp
	mov %esp, %ebp
	push 28(%ebp)
	push 24(%ebp)
	push 20(%ebp)
	push 16(%ebp)
	push 12(%ebp)
	call *8(%ebp)
	add $20, %esp
	leave
	ret
	

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
