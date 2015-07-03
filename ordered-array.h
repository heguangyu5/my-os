#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "common.h"

typedef s8int(*lessthan_predicate_t)(void *, void *);
typedef struct {
	void **arr;
	u32int size;
	u32int max_size;
	lessthan_predicate_t less_than;
} ordered_array_t;

ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void *addr, u32int max_size, lessthan_predicate_t less_than);

void destroy_ordered_array(ordered_array_t *array);
void insert_ordered_array(void *item, ordered_array_t *array);
void *lookup_ordered_array(u32int idx, ordered_array_t *array);
void remove_ordered_array(u32int idx, ordered_array_t *array);
void remove_ordered_array_item(void *item, ordered_array_t *array);

#endif
