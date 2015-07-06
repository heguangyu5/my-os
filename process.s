.global copy_page_physical
.global read_eip

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
