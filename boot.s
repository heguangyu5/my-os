/* @see grub-0.97/stage2/mb_header.h */
.equ MULTIBOOT_MAGIC, 0x1BADB002
.equ MULTIBOOT_PAGE_ALIGN, 0x00000001
.equ MULTIBOOT_MEMORY_INFO, 0x00000002
.equ MULTIBOOT_HEADER_FLAGS, MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO 
.equ MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_HEADER_FLAGS)

.code32

.extern code
.extern bss
.extern end
.extern main

.global multiboot
multiboot:
	.int MULTIBOOT_MAGIC
	.int MULTIBOOT_HEADER_FLAGS
	.int MULTIBOOT_CHECKSUM
	
	.int multiboot
	.int code
	.int bss
	.int end
	.int _start


.global _start
_start:
	push %esp
	push %ebx

	cli
	call main
	jmp .
