#include "kernel/include/data-structures/heap/binary-heap.h"
#include "kernel/include/mm/mm.h"

#define PARENT(i)	((i) ? ((i) - 1) >> 1 : 0)
#define LEFT(i)		(1 + ((i) << 1))
#define RIGHT(i)	(2 + ((i) << 1))

static void k_binary_heap_to_up(struct k_binary_heap *heap, unsigned int index)
{
	void *temp;

	while (index > 0 &&
			heap->compare(heap->elements[PARENT(index)],
					heap->elements[index])) {
		temp = heap->elements[index];
		heap->elements[index] = heap->elements[PARENT(index)];
		heap->elements[PARENT(index)] = temp;

		index = PARENT(index);
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

