#include "common.h"

typedef struct {
	u16int limit_low;
	u16int base_low;
	u8int base_middle;
	u8int access;
	u8int granularity;
	u8int base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
	u16int size;
	u32int base;
} __attribute__ ((packed)) gdt_ptr_t;

typedef struct {
	u16int base_low;
	u16int sel;
	u8int always0;
	u8int flags;
	u16int base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
	u16int size;
	u32int base;
} __attribute__((packed)) idt_ptr_t;

typedef struct {
	u32int prev_tss;
	u32int esp0;
	u32int ss0;
	u32int esp1;
	u32int ss1;
	u32int esp2;
	u32int ss2;
	u32int cr3;
	u32int eip;
	u32int eflags;
	u32int eax;
	u32int ecx;
	u32int edx;
	u32int ebx;
	u32int esp;
	u32int ebp;
	u32int esi;
	u32int edi;
	u32int es;
	u32int cs;
	u32int ss;
	u32int ds;
	u32int fs;
	u32int gs;
	u32int ldt;
	u16int trap;
	u16int iomap_base;
} __attribute__((packed)) tss_entry_t;

void init_gdt();
void init_idt();
