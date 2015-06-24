#include "idt.h"

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void init_idt()
{
	idt_ptr.size = sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base = (u32int)&idt_entries;
	
	idt_set(0, (u32int)isr0, 0x08, 0x8E);
	idt_set(1, (u32int)isr1, 0x08, 0x8E);
	idt_set(2, (u32int)isr2, 0x08, 0x8E);

	idt_flush((u32int)&idt_ptr);
}

void idt_set(u8int idx, u32int base, u16int sel, u8int flags)
{
	idt_entries[idx].base_low  = base & 0xFFFF;
	idt_entries[idx].base_high = (base >> 16) & 0xFFFF;

	idt_entries[idx].sel 	 = sel;
	idt_entries[idx].always0 = 0;

	idt_entries[idx].flags = flags;
}
