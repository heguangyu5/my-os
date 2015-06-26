.global switch_page_directory
switch_page_directory:
	mov 4(%esp), %eax
	mov %eax, %cr3
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	ret
