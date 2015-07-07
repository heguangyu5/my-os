#ifndef TASK_H
#define TASK_H

#include "common.h"
#include "paging.h"

typedef struct task {
	u32int id;
	u32int esp;
	u32int ebp;
	u32int eip;
	page_directory_t *page_directory;
	struct task *next;
	u32int interrupt;
} task_t;

void init_tasking();
void switch_task();
int fork();
void move_stack(void *new_stack_start, u32int size);
int getpid();

#endif
