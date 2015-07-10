.global copy_page_physical
.global read_eip
.global do_switch_task
.global do_switch_to_user_mode

copy_page_physical:
	push %ebx
	pushf

	cli

	mov 12(%esp), %ebx
	mov 16(%esp), %ecx

	mov %cr0, %edx
	and $0x7fffffff, %edx
	mov %edx, %cr0

	mov $1024, %edx
loop:
	mov (%ebx), %eax
	mov %eax, (%ecx)
	add $4, %ebx
	add $4, %ecx
	dec %edx
	jnz loop

	mov %cr0, %edx
	or $0x80000000, %edx
	mov %edx, %cr0

	popf
	pop %ebx
	ret

read_eip:
	pop %eax
	jmp *%eax

do_switch_task:
	mov %esp, %eax
	mov 4(%eax), %ecx 	# eip
	mov 8(%eax), %esp 	# esp
	mov 12(%eax), %ebp  # ebp
	mov 16(%eax), %edx  # page_directory
	mov 20(%eax), %eax  # interrupt_before
	mov %edx, %cr3
	cmp $1, %eax
	je done
	sti
done:
	jmp *%ecx

do_switch_to_user_mode:
	cli
	mov $0x23, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %esp, %eax
	pushl $0x23
	pushl %eax
	pushf
	pop %ebx
	or $0x200, %ebx
	push %ebx
	pushl $0x1b
	push (%eax)
	iret
