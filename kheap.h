#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"
#include "ordered-array.h"
#include "paging.h"

u32int kmalloc_a(u32int size);
u32int kmalloc_p(u32int size, u32int *phys);
u32int kmalloc_ap(u32int size, u32int *phys);
u32int kmalloc(u32int size);
void kfree(void *p);
void print_placement_address();
u32int virt2phys(u32int virt_addr);

#define KHEAP_START 0xC0000000
#define KHEAP_INIT_SIZE 0x100000

#define HEAP_HOLE_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB

typedef struct {
	u32int magic;
	u8int is_hole;
	u32int size;
} header_t;

typedef struct {
	u32int magic;
	header_t *header;
} footer_t;

typedef struct {
	ordered_array_t holes;
	u32int start_addr;
	u32int end_addr;
	u32int max_addr;
	u8int supervisor;
	u8int readonly;
} heap_t;

heap_t *create_heap(u32int start_addr, u32int end_addr, u32int max_addr, u8int supervisor, u8int readonly);
void *alloc(u32int size, u8int page_align, heap_t *heap);
void free(void *p, heap_t *heap);
void print_heap(heap_t *heap);
void print_holes(heap_t *heap);

#endif
