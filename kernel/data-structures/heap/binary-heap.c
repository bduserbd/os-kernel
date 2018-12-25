#include "kernel/include/data-structures/heap/binary-heap.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/string.h"

#define PARENT(i)	((i) ? ((i) - 1) >> 1 : 0)
#define LEFT(i)		(1 + ((i) << 1))
#define RIGHT(i)	(2 + ((i) << 1))

static bool k_binary_heap_compare(struct k_binary_heap *heap, void *item1, void *item2)
{
	if (!item1 && !item2)
		return false;

	if (heap->type == K_BINARY_HEAP_MAX) {
		if (!item1 && item2)
			return true;
		else if (item1 && !item2)
			return false;
		else
			return heap->compare(item1, item2) == 1;
	} else {
		if (!item1 && item2)
			return true;
		else if (item1 && !item2)
			return false;
		else
			return heap->compare(item1, item2) == -1;
	}

}

static void k_binary_heap_to_up(struct k_binary_heap *heap, unsigned int index)
{
	void *temp;

	while (index > 0 &&
			k_binary_heap_compare(heap, heap->elements[PARENT(index)],
				heap->elements[index])) {
		temp = heap->elements[index];
		heap->elements[index] = heap->elements[PARENT(index)];
		heap->elements[PARENT(index)] = temp;

		index = PARENT(index);
	}
}

static void k_binary_heap_to_down(struct k_binary_heap *heap,
		unsigned int start_index, unsigned int end_index)
{
	void *temp;
	unsigned int swap;

	while (LEFT(start_index) < end_index) {
		swap = LEFT(start_index);

		if (swap + 1 != end_index)
			if (k_binary_heap_compare(heap, heap->elements[swap], heap->elements[swap + 1]))
				swap++;

		if (k_binary_heap_compare(heap, heap->elements[start_index], heap->elements[swap])) {
			temp = heap->elements[start_index];
			heap->elements[start_index] = heap->elements[swap];
			heap->elements[swap] = temp;
		}

		start_index = swap;
	}
}

void k_binary_heap_insert(struct k_binary_heap *heap, void *item)
{
	unsigned int next_index;

	if (!item)
		return;

	next_index = heap->index + 1;
	if (next_index == heap->total_elements)
		return;

	heap->elements[next_index] = item;
	k_binary_heap_to_up(heap, next_index);

	heap->index = next_index;
}

void *k_binary_heap_get_root(struct k_binary_heap *heap)
{
	if (!heap)
		return NULL;

	if (!heap->elements || !heap->index)
		return NULL;

	return heap->elements[0];
}

void *k_binary_heap_fetch_root(struct k_binary_heap *heap)
{
	void *item;

	if (heap->index == 0)
		return NULL;

	item = k_binary_heap_get_root(heap);

	heap->elements[0] = heap->elements[heap->index];
	heap->elements[heap->index] = NULL;
	k_binary_heap_to_down(heap, 0, heap->index);

	heap->index--;

	return item;
}

struct k_binary_heap *k_binary_heap_init(int type, unsigned int total_elements,
		int (*compare)(void *, void *))
{
	unsigned int i;
	struct k_binary_heap *heap;

	if (type != K_BINARY_HEAP_MIN && type != K_BINARY_HEAP_MAX)
		return NULL;

	heap = k_malloc(sizeof(struct k_binary_heap));
	if (!heap)
		return NULL;

	heap->type = type;

	heap->total_elements = total_elements;
	heap->index = 0;

	heap->elements = k_malloc(heap->total_elements * sizeof(void *));
	if (!heap->elements) {
		k_free(heap);
		return NULL;
	}

	for (i = 0; i < heap->total_elements; i++)
		heap->elements[i] = NULL;

	heap->compare = compare;

	return heap;
}

void k_binary_heap_uninit(struct k_binary_heap *heap)
{
	int i;

	if (!heap)
		return;

	if (heap->elements) {
		for (i = 0; i < heap->total_elements; i++)
			heap->elements[i] = NULL;

		k_free(heap->elements);
	}

	k_memset(heap, 0, sizeof(struct k_binary_heap));

	k_free(heap);
}

