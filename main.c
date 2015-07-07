#include "multiboot.h"
#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"
#include "kheap.h"
#include "paging.h"
#include "fs.h"
#include "task.h"

extern u32int end;
extern u32int placement_address;
extern heap_t *kheap;
extern fs_node_t *initrd_root;
extern fs_node_t *initrd_dev;
extern fs_node_t *root_nodes;

u32int initial_esp;

int main(struct multiboot *mboot_ptr, u32int initial_stack)
{
	initial_esp = initial_stack;

	init_gdt();
	init_idt();

	monitor_clear();

	asm volatile("sti");
	init_timer(50); // 50Hz

	monitor_write("kernel end at ");
	monitor_write_hex((u32int)&end);
	monitor_put('\n');
	monitor_write("main stack at ");
	monitor_write_hex(initial_esp);
	monitor_put('\n');
	break_point();

    ASSERT(mboot_ptr->mods_count > 0);
	u32int initrd_location = mboot_ptr->mods_addr[0];
	u32int initrd_end	   = mboot_ptr->mods_addr[1];
	placement_address = initrd_end;
	monitor_write("initrd: ");
	monitor_write_hex(initrd_location);
	monitor_write(" ~ ");
	monitor_write_hex(initrd_end);
	monitor_put('\n');
	print_placement_address();
	break_point();

	init_paging();
	monitor_write("paging enabled\n");
	print_heap(kheap);
	break_point();

	init_tasking();
	monitor_write("multi task ready\n");
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
