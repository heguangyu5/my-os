#include "kheap.h"
#include "monitor.h"
#include "paging.h"

extern u32int end;
u32int placement_address = (u32int)&end;
extern heap_t *kheap;
extern page_directory_t *kernel_directory;

static u32int kmalloc_internal(u32int size, int align, u32int *phys)
{
	if (align && (placement_address & 0xFFF)) {
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}
	if (phys) {
		*phys = placement_address;
	}
	u32int tmp = placement_address;
	placement_address += size;
	return tmp;
}

u32int kmalloc_a(u32int size)
{
	if (kheap == 0) {
		return kmalloc_internal(size, 1, 0);
	}
	return (u32int)alloc(size, 1, kheap);
}

u32int kmalloc_p(u32int size, u32int *phys)
{
	if (kheap == 0) {
		return kmalloc_internal(size, 0, phys);
	}
	*phys = (u32int)alloc(size, 0, kheap);
	return *phys;
}

u32int kmalloc_ap(u32int size, u32int *phys)
{
	if (kheap == 0) {
		return kmalloc_internal(size, 1, phys);
	}
	*phys = (u32int)alloc(size, 1, kheap);
	return *phys;
}

u32int kmalloc(u32int size)
{
	if (kheap == 0) {
		return kmalloc_internal(size, 0, 0);
	}
	return (u32int)alloc(size, 0, kheap);
}

void kfree(void *p) 
{
	ASSERT(kheap != 0);
	free(p, kheap);
}

void print_kheap_brk()
{
	monitor_write("current placement_address = ");
	monitor_write_hex(placement_address);
	monitor_put('\n');
}


static s8int header_t_less_than(void *a, void *b)
{
	return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

static void new_hole(u32int start, u32int size, heap_t *heap)
{
	header_t *header = (header_t *)start;
	header->magic 	 = HEAP_MAGIC;
	header->is_hole	 = 1;
	header->size	 = size;

	footer_t *footer = (footer_t *)(start + size - sizeof(footer_t));
	footer->magic	 = HEAP_MAGIC;
	footer->header	 = header;

	insert_ordered_array((void *)start, &heap->holes);
}

heap_t *create_heap(u32int start_addr, u32int end_addr, u32int max_addr, u8int supervisor, u8int readonly)
{
	ASSERT(start_addr % 0x1000 == 0);
	ASSERT(end_addr % 0x1000 == 0);

	heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
	heap->holes = place_ordered_array((void *)start_addr, HEAP_HOLE_SIZE, &header_t_less_than);

	start_addr += sizeof(void *) * HEAP_HOLE_SIZE;
	if (start_addr & 0xFFF) {
		start_addr &= 0xFFFFF000;	
		start_addr += 0x1000;
	}

	heap->start_addr = start_addr;
	heap->end_addr	 = end_addr;
	heap->max_addr	 = max_addr;
	heap->supervisor = supervisor;
	heap->readonly	 = readonly;

	new_hole(start_addr, end_addr - start_addr, heap);

	return heap;
}

static s32int find_smallest_hole(u32int size, u8int page_align, heap_t *heap)
{
	u32int idx = 0;
	ordered_array_t *holes = &heap->holes;
	while (idx < holes->size) {
		header_t *header = (header_t *)lookup_ordered_array(idx, holes);
		if (page_align) {
			u32int location = (u32int)header;
			u32int offset = 0;
			if ((location + sizeof(header_t)) & 0xFFF) {
				offset = 0x1000 - (location + sizeof(header_t)) & 0xFFF;
			}
			s32int hole_size = (s32int)header->size - offset;
			if (hole_size >= (s32int)size) {
				break;
			}
		} else {
			if (header->size >= size) {
				break;
			}
		}
		idx++;
	}

	if (idx == holes->size) {
		return -1;
	}
	return idx;
}

static void joinLeft(u32int start, u32int size, heap_t *heap)
{
	footer_t *left_footer = (footer_t *)(start - sizeof(footer_t));
	header_t *left_header = left_footer->header;
	if (left_header->is_hole || size < sizeof(header_t) + sizeof(footer_t) + 4) {
		left_header->size += size;
		left_footer = (footer_t *)(start + size - sizeof(footer_t));
		left_footer->magic = HEAP_MAGIC;
		left_footer->header = left_header;
		return;
	}

	new_hole(start, size, heap);
}

static void expand(u32int incr_size, heap_t *heap)
{
	ASSERT(incr_size > 0);

	if (incr_size & 0xFFF) {
		incr_size &= 0xFFFFF000;
		incr_size += 0x1000;
	}

	ASSERT(heap->end_addr + incr_size <= heap->max_addr);

	int i = 0;
	while (i < incr_size) {
		alloc_frame(
			get_page(heap->end_addr + i, 1, kernel_directory),
			heap->supervisor ? 1 : 0,
			heap->readonly ? 1 : 0
		);
		i += 0x1000;
	}
	
	joinLeft(heap->end_addr, incr_size, heap);

	heap->end_addr += incr_size;
}

void *alloc(u32int size, u8int page_align, heap_t *heap)
{
	u32int new_size = size + sizeof(header_t) + sizeof(footer_t);
	s32int idx = find_smallest_hole(new_size, page_align, heap);

	if (idx == -1) {
		expand(new_size, heap);
		return alloc(size, page_align, heap);
	}

	header_t *header = (header_t *)lookup_ordered_array(idx, &heap->holes);
	u32int hole_start = (u32int)header;
	u32int hole_size  = header->size;

	if (page_align && (hole_start + sizeof(header_t)) & 0xFFF) {
		u32int offset = 0x1000 - (hole_start + sizeof(header_t)) & 0xFFF;
		joinLeft(hole_start, offset, heap);
		hole_start += offset;
		hole_size -= offset;
	}

	// 如果余下的空间可以存下4个字节(一个int值),则生成一个新hole,
	// 否则,全都分配出去
	u8int leftAsNewHole = 1;
	if (hole_size - new_size < sizeof(header_t) + sizeof(footer_t) + 4) {
		new_size = hole_size;
		leftAsNewHole = 0;
	}
	
	remove_ordered_array(idx, &heap->holes);

	header_t *block_header = (header_t *)hole_start;
	block_header->magic   = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size    = new_size; 

	footer_t *block_footer = (footer_t *)(hole_start + new_size - sizeof(footer_t));
	block_footer->magic  = HEAP_MAGIC;
	block_footer->header = block_header;

	if (leftAsNewHole) {
		new_hole(hole_start + new_size, hole_size - new_size, heap);
	}

	return (void *)((u32int)block_header + sizeof(header_t));
}

void free(void *p, heap_t *heap)
{
	if (p == 0) {
		return;
	}

	header_t *header = (header_t *)((u32int)p - sizeof(header_t));
	footer_t *footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));

	ASSERT(header->magic == HEAP_MAGIC);
	ASSERT(footer->magic == HEAP_MAGIC);

	joinLeft((u32int)header, header->size, heap);

	u32int next_header = (u32int)header + header->size;
	if (next_header < heap->end_addr) {
		joinLeft(next_header, ((header_t *)next_header)->size, heap);
	}
}
