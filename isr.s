.code32

.macro ISR_NOERRCODE idx
.global isr\idx
isr\idx:
	cli
	pushb 0
	pushb \idx
	jmp isr_common_stub
.endm

.macro ISR_ERRCODE idx
.global isr\idx
isr\idx:
	cli
	pushb \idx
	jmp isr_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
