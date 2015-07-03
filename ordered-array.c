#include "ordered-array.h"

ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than)
{
	ordered_array_t ret;
	ret.arr = (void *)kmalloc(max_size * sizeof(void *));
	memset(ret.arr, 0, max_size * sizeof(void *));
	ret.size = 0;
	ret.max_size = max_size;
	ret.less_than = less_than;
	return ret;
}

ordered_array_t place_ordered_array(void *addr, u32int max_size, lessthan_predicate_t less_than)
{
	ordered_array_t ret;
	ret.arr = (void **)addr;
	memset(ret.arr, 0, max_size * sizeof(void *));
	ret.size = 0;
	ret.max_size = max_size;
	ret.less_than = less_than;
	return ret;
}

void destroy_ordered_array(ordered_array_t *array)
{
	// todo
}

void insert_ordered_array(void *item, ordered_array_t *array)
{
	ASSERT(array->size < array->max_size);
	ASSERT(array->less_than);

	u32int idx = 0;
	while (idx < array->size && array->less_than(array->arr[idx], item)) {
		idx++;
	}
	if (idx == array->size) {
		array->arr[idx] = item;
		array->size++;
	} else {
		void *cur = array->arr[idx];
		array->arr[idx] = item;
		while (idx < array->size) {
			idx++;
			void *next = array->arr[idx];
			array->arr[idx] = cur;
			cur = next;
		}
		array->size++;
	}
}

void *lookup_ordered_array(u32int idx, ordered_array_t *array)
{
	ASSERT(idx < array->size);
	return array->arr[idx];
}

void remove_ordered_array(u32int idx, ordered_array_t *array)
{
	ASSERT(idx < array->size);

	while (idx < array->size) {
			array->arr[idx] = array->arr[idx+1];
			idx++;
	}
	array->size--;
}

void remove_ordered_array_item(void *item, ordered_array_t *array)
{
    u32int idx = 0;
    while (idx < array->size) {
        if (item == array->arr[idx]) {
            remove_ordered_array(idx, array);
            break;
        }
        idx++;
    }
}
