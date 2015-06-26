#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"

extern u32int placement_address;

u32int kmalloc_a(u32int size);
u32int kmalloc_p(u32int size, u32int *phys);
u32int kmalloc_ap(u32int size, u32int *phys);
u32int kmalloc(u32int size);
void print_kheap_brk();

#endif
