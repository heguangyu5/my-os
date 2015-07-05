#include "multiboot.h"
#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"
#include "kheap.h"
#include "paging.h"
#include "fs.h"

extern u32int end;
extern u32int placement_address;
extern heap_t *kheap;
extern fs_node_t *initrd_root;
extern fs_node_t *initrd_dev;
extern fs_node_t *root_nodes;

int main(struct multiboot *mboot_ptr)
{
	init_gdt();
	init_idt();

	monitor_clear();
	monitor_write("Hello, World!\n");

	asm volatile("int $0x3");
	asm volatile("int $0x4");

	//asm volatile("sti");
	//init_timer(50); // 50Hz

	monitor_write("kernel end at ");
	monitor_write_hex((u32int)&end);
	monitor_put('\n');
	break_point();

	monitor_write("mboot_ptr = ");
	monitor_write_hex((u32int)mboot_ptr);
	monitor_put('\n');
	monitor_write("&mboot_ptr->mods_addr = ");
	monitor_write_hex((u32int)&mboot_ptr->mods_addr);
	monitor_put('\n');
	monitor_write("mboot_ptr->mods_addr = ");
	monitor_write_hex(mboot_ptr->mods_addr);
	monitor_put('\n');
	ASSERT(mboot_ptr->mods_count > 0);
	u32int initrd_location = *((u32int *)mboot_ptr->mods_addr);
	u32int initrd_end	   = *(u32int *)(mboot_ptr->mods_addr + 4);
	placement_address = initrd_end;

	u32int addr;
	print_placement_address();
	break_point();

	addr = kmalloc(8);
	monitor_write("kmalloc(8) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_placement_address();
	break_point();

	addr = kmalloc_a(0x1000);
	monitor_write("kmalloc_a(0x1000) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_placement_address();
	break_point();

	addr = kmalloc_a(8);
	monitor_write("kmalloc_a(8) = ");
	monitor_write_hex(addr);
	monitor_put('\n');
	print_placement_address();
	break_point();

	init_paging();
	monitor_write("Hello, paging world!\n");
	print_heap(kheap);
	break_point();

	u32int *a = (u32int *)kmalloc(8);
	u32int *b = (u32int *)kmalloc(8);
	u32int *c = (u32int *)kmalloc(8);
	u32int *d = (u32int *)kmalloc(8);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	kfree(d);
	print_heap(kheap);
	print_holes(kheap);
	break_point();
	kfree(b);
	print_heap(kheap);
	print_holes(kheap);
	break_point();
	kfree(c);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	u32int *e = alloc(12, 0, kheap);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	u32int *f = alloc(512, 1, kheap);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	u32int *g = (u32int *)kmalloc(200);
	print_heap(kheap);
	print_holes(kheap);
	break_point();

	monitor_write("init initrd\n");
	init_initrd(initrd_location);
	monitor_write("initrd_root at ");
	monitor_write_hex((u32int)initrd_root);
	monitor_put('\n');
	monitor_write("initrd_dev at ");
	monitor_write_hex((u32int)initrd_dev);
	monitor_put('\n');
	monitor_write("root_nodes at ");
	monitor_write_hex((u32int)root_nodes);
	monitor_put('\n');
	print_heap(kheap);
	break_point();

	int i = 0;
	struct dirent *node = 0;
	while ((node = readdir_fs(initrd_root, i)) != 0) {
		monitor_write("Found file ");
		monitor_write(node->name);
		fs_node_t *fsnode = finddir_fs(initrd_root, node->name);
		monitor_write(" node at ");
		monitor_write_hex((u32int)fsnode);
		monitor_write(" flag at ");
		monitor_write_hex((u32int)&fsnode->flags);
		monitor_write(" is ");
		monitor_write_hex(fsnode->flags);
		break_point();

		if ((fsnode->flags & 0x7) == FS_DIRECTORY) {
			monitor_write("\n\t(directory)\n");
		} else {
			monitor_write("\n\t contents: \n\"");
			char buf[256];
			u32int sz = read_fs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++) {
				monitor_put(buf[j]);
			}
			monitor_write("\"\n");
		}
		i++;
	}

	return 0;
}
