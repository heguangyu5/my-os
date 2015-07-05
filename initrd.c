#include "initrd.h"
#include "kheap.h"

initrd_header_t *initrd_header;
initrd_file_header_t *file_headers;
fs_node_t *initrd_root;
fs_node_t *initrd_dev;
fs_node_t *root_nodes;
int nroot_nodes;

static struct dirent dirent;

static u32int initrd_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer)
{
	initrd_file_header_t header = file_headers[node->inode];
	if (offset > header.length) {
		return 0;
	}
	if (offset + size > header.length) {
		size = header.length - offset;
	}
	memcpy(buffer, (u8int *)(header.offset + offset), size);
	return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, u32int index)
{
	if (node == initrd_root && index == 0) {
		char *devname = "dev";
		memset(&dirent, 0, sizeof(struct dirent));
		memcpy(dirent.name, devname, 3);
		dirent.ino = 0;
		return &dirent;
	}

	if (index - 1 >= nroot_nodes) {
		return 0;
	}

	memset(&dirent, 0, sizeof(struct dirent));
	memcpy(dirent.name, root_nodes[index-1].name, 64);
	dirent.ino = root_nodes[index-1].inode;
	return &dirent;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
	if (node == initrd_root && strcmp(name, "dev") == 0) {
		return initrd_dev;
	}

	int i;
	for (i = 0; i < nroot_nodes; i++) {
		if (strcmp(name, root_nodes[i].name) == 0) {
			return &root_nodes[i];
		}
	}

	return 0;
}

fs_node_t *init_initrd(u32int location)
{
	initrd_header = (initrd_header_t *)location;
	file_headers = (initrd_file_header_t *)(location + sizeof(initrd_header_t));

	initrd_root = (fs_node_t *)kmalloc(sizeof(fs_node_t));
	memset(initrd_root, 0, sizeof(fs_node_t));
	char rootname[] = "initrd";
	memcpy(initrd_root->name, rootname, 6);
	initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
	initrd_root->flags = FS_DIRECTORY;
	initrd_root->read = 0;
	initrd_root->write = 0;
	initrd_root->open = 0;
	initrd_root->close = 0;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
	initrd_root->ptr = 0;
	initrd_root->impl = 0;

	initrd_dev = (fs_node_t *)kmalloc(sizeof(fs_node_t));
	memset(initrd_dev, 0, sizeof(fs_node_t));
	char devname[] = "dev";
	memcpy(initrd_dev, devname, 3);
	initrd_dev->flags = FS_DIRECTORY;
	initrd_dev->readdir = &initrd_readdir;
	initrd_dev->finddir = &initrd_finddir;

	root_nodes = (fs_node_t *)kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
	memset(root_nodes, 0, sizeof(fs_node_t) * initrd_header->nfiles);
	nroot_nodes = initrd_header->nfiles;

	int i;
	for (i = 0; i < initrd_header->nfiles; i++) {
		file_headers[i].offset += location;
		memcpy(root_nodes[i].name, file_headers[i].name, 64);
		root_nodes[i].length = file_headers[i].length;
		root_nodes[i].inode = i;
		root_nodes[i].flags = FS_FILE;
		root_nodes[i].read = &initrd_read;
	}

	return initrd_root;
}
