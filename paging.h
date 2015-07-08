#ifndef PAGING_H
#define PAGING_H

#include "common.h"
#include "isr.h"
#include "kheap.h"

typedef struct {
	u32int present	: 1;
	u32int rw		: 1;
	u32int user		: 1;
	u32int accessed	: 1;
	u32int dirty	: 1;
	u32int unused	: 7;
	u32int frame	: 20;
} page_t;

typedef struct {
	page_t pages[1024];
} page_table_t;

typedef struct {
	page_table_t *tables[1024];
	u32int tablesPhysical[1024];
	u32int physicalAddr;
} page_directory_t;

void init_paging();
void switch_page_directory(u32int pageDirPhysAddr);
page_t *get_page(u32int address, int make, page_directory_t *dir);
void page_fault(registers_t regs);
page_directory_t *clone_directory(page_directory_t *src);
void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void print_page_direcotry(page_directory_t *dir, u8int printPageTable);

#endif
