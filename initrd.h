#ifndef INITRD_H
#define INITRD_H

#include "fs.h"

typedef struct {
	u32int nfiles;
} initrd_header_t;

typedef struct {
	u8int magic;
	u8int name[64];
	u32int offset;
	u32int length;
} initrd_file_header_t;

fs_node_t *init_initrd(u32int location);

#endif
