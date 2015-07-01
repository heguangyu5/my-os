#include "paging.h"
#include "kheap.h"

static void set_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void clear_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static u32int test_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

static u32int first_frame()
{
	u32int i, j;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] != 0xFFFFFFFF) {
			for (j = 0; j < 32; j++) {
				u32int toTest = 0x1 << j;
				if (!(frames[i] & toTest)) {
					return i * 32 + j;
				}
			}
		}
	}

	return (u32int)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	if (page->frame != 0) {
		return;
	}

	u32int idx = first_frame();
	if (idx == (u32int)-1) {
		// no free frames!!	
	}
	set_frame(idx * 0x1000);
	page->present = 1;
	page->rw  = is_writeable ? 1 : 0;
	page->user = is_kernel ? 0 : 1;
	page->frame = idx;
}

void free_frame(page_t *page)
{
	u32int frame = page->frame;
	if (!frame) {
		return;
	}

	clear_frame(frame);
	page->frame = 0;
}

void init_paging()
{
	u32int total_mem = 0x1000000; // 16M

	nframes = total_mem / 0x1000; // 4K
	frames = (u32int *)kmalloc(nframes / 8);
	memset(frames, 0, nframes / 8);
print_kheap_brk();

	kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
	current_directory = kernel_directory;
print_kheap_brk();

	int i = 0;
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
		// 如果在这里alloc_frame,就会使frames bitmap的前几个map到heap上
		// 这样phys addr != virt addr了
		get_page(i, 1, kernel_directory);
	}

	// 这里之所以要加0x1000,多map出来4K的内存,是为了启用page后,heap创建前,需要一块内存
	// 保存heap_t,如果不多map出这4K,就会导致heap holes这个ordered array覆盖到heap_t的数据
	i = 0;
	while (i < placement_address + 0x1000) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}
print_kheap_brk();
	// 到此,kernel使用的内存都分配了,并且phys addr == virt addr

	// 从这里开始,把heap的virt地址开始map到phys地址上, phys addr != virt addr
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
	}

	register_interrupt_handler(14, page_fault);

	switch_page_directory((u32int)&kernel_directory->tablesPhysical);

	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INIT_SIZE, 0xCFFFF000, 0, 0);
}

page_t *get_page(u32int address, int make, page_directory_t *dir)
{
	address /= 0x1000;
	u32int table_idx = address / 1024;
	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[address % 1024];
	}

	if (make) {
		u32int tmp;
		dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
		dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT RW US
		return &dir->tables[table_idx]->pages[address % 1024];
	}

	return 0;
}

void page_fault(registers_t regs)
{
	u32int faulting_address;
	asm volatile ("mov %%cr2, %0" : "=r"(faulting_address));

	int present = !(regs.err_code & 0x1);
	int rw = regs.err_code & 0x02;
	int us = regs.err_code & 0x04;
	int reserved = regs.err_code & 0x08;
	int id = regs.err_code & 0x10;

	monitor_write("Page fault! (");
	if (present) {
		monitor_write("present ");
	}
	if (rw) {
		monitor_write("read-only ");
	}
	if (us) {
		monitor_write("user-mode ");
	}
	if (reserved) {
		monitor_write("reserved ");
	}
	if (id) {
		monitor_write("instruction fetch");
	}
	monitor_write(") at ");
	monitor_write_hex(faulting_address);
	monitor_put('\n');
	PANIC("Page fault");
}
