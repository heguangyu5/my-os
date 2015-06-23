#include "gdt.h"

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

static void gdt_set(u32int idx, u32int base, u32int limit, u8int access, u8int gran)
{
	gdt_entries[idx].base_low	 = (base & 0xFFFF);
	gdt_entries[idx].base_middle = (base >> 16) & 0xFF;
	gdt_entries[idx].base_high	 = (base >> 24) & 0xFF;

	gdt_entries[idx].limit_low	 = (limit & 0xFFFF);
	gdt_entries[idx].granularity = (limit >> 16) & 0x0F;

	gdt_entries[idx].granularity |= gran & 0xF0;
	gdt_entries[idx].access		 = access;
}

void init_gdt()
{	
	gdt_ptr.size = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base = (u32int)&gdt_entries;

	gdt_set(0, 0, 0, 0, 0);					// Null segment
	gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	// Code segment
	gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	// Data segment
	gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);	// User mode code segment
	gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);	// User mode data segment

	gdt_flush((u32int)&gdt_ptr);
}
