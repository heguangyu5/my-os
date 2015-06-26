#include "kheap.h"
#include "monitor.h"

extern u32int end;
u32int placement_address = (u32int)&end;

static u32int kmalloc_internal(u32int size, int align, u32int *phys)
{
	if (align && (placement_address & 0xFFFFF000)) {
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
	return kmalloc_internal(size, 1, 0);
}

u32int kmalloc_p(u32int size, u32int *phys)
{
	return kmalloc_internal(size, 0, phys);
}

u32int kmalloc_ap(u32int size, u32int *phys)
{
	return kmalloc_internal(size, 1, phys);
}

u32int kmalloc(u32int size)
{
	return kmalloc_internal(size, 0, 0);
}

void print_kheap_brk()
{
	monitor_write("current placement_address = ");
	monitor_write_hex(placement_address);
	monitor_put('\n');
}
