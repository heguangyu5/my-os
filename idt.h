#include "common.h"

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

void init_idt();
