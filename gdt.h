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

void init_gdt();
