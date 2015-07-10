#ifndef TASK_H
#define TASK_H

#include "common.h"
#include "paging.h"

#define KERNEL_STACK_SIZE 2048

typedef struct task {
	u32int id;
	u32int esp;
	u32int ebp;
	u32int eip;
	page_directory_t *page_directory;
	struct task *next;
	u32int interrupt;
	u32int kernel_stack;
} task_t;

void init_tasking();
void switch_task();
int fork();
void move_stack(void *new_stack_start, u32int size);
int getpid();
void print_task(volatile task_t *task);
void switch_to_user_mode();

#endif
