#include "include/mm/buddy.h"
#include "include/string.h"

static struct k_buddy_node *k_group = NULL;
static struct k_buddy_node *k_node = NULL;
static k_uint8_t *k_heap = NULL;

void __attribute__((weak)) k_paging_reserve_pages(k_uint32_t start, k_uint32_t range)
{

}

static struct k_buddy_node *k_buddy_get(struct k_buddy_node *node)
{
	unsigned long a;

	a = (node - k_node) << K_BUDDY_MIN_BLOCK_LOG2;

	return &k_node[(a ^ (1 << node->order)) >> K_BUDDY_MIN_BLOCK_LOG2];
}

static int k_buddy_best_fit_group(k_size_t size)
{
	int i;

	for (i = 0; i < K_BUDDY_TOTAL_GROUPS; i++)
		if (size > (1  << (K_BUDDY_MAX_BLOCK_LOG2 - i - 1)))
			return i;

	return i - 1;
}

static k_uint8_t *k_buddy_node_to_address(struct k_buddy_node *node)
{
	return k_heap + ((node - k_node) << K_BUDDY_MIN_BLOCK_LOG2);
}

static struct k_buddy_node *k_buddy_address_to_node(void *ptr)
{
	unsigned long a;

	a = (unsigned long)ptr;

	if (a & ((1 << K_BUDDY_MIN_BLOCK_LOG2) - 1))
		return NULL;

	if (a < (unsigned long)k_heap ||
			(unsigned long)k_heap + (1 << K_BUDDY_MAX_BLOCK_LOG2) < a)
		return NULL;

	return &k_node[(a - (unsigned long)k_heap) >> K_BUDDY_TOTAL_GROUPS];
}

void k_buddy_free(void *ptr)
{
	k_uint8_t order;
	struct k_buddy_node *a, *b;

	a = k_buddy_address_to_node(ptr);
	if (!a)
		return;

	order = a->order;

	while (1) {
		b = k_buddy_get(a);

		if (order == K_BUDDY_MAX_BLOCK_LOG2 ||
				b->status == K_BUDDY_NODE_USED ||
				(b->status == K_BUDDY_NODE_SPLIT && b->order != order))
			break;

		b->prev->next = b->next;
		b->next->prev = b->prev;

		order++;

		if (a > b)
			a = b;
	}

	a->status = K_BUDDY_NODE_SPLIT;
	b = k_group[order].next;
	a->next = b;
	b->prev = a;
	a->order = order;
	a->prev = &k_group[order];
	k_group[order].next = a;
}

void *k_buddy_alloc(k_size_t size)
{
	int i;
	int group;
	struct k_buddy_node *a, *b;

	if (!k_group || !k_node || !k_heap)
		return NULL;

	group = k_buddy_best_fit_group(size);

	i = K_BUDDY_TOTAL_GROUPS;
	while (1) {
		i--;

		if ((k_group[i].next == &k_group[i]) || (size > (1 << k_group[i].order)))
			continue;

		a = k_group[i].next;
		b = a->next;
		k_group[i].next = b;
		b->prev = &k_group[i];

		a->status = K_BUDDY_NODE_USED;		

		break;
	}

	while (1) {
		if (i == group)
			return k_buddy_node_to_address(a);

		i++;
		a->order--;

		b = k_buddy_get(a);
		b->status = K_BUDDY_NODE_SPLIT;
		b->order = a->order;

		b->next = b->prev = &k_group[i];
		k_group[i].next = k_group[i].prev = b;
	}
}

void k_buddy_init(k_uint32_t heap)
{
	int i;

	if (k_group || k_node || k_heap)
		return;

	k_group = (void *)heap;
	k_paging_reserve_pages(heap, K_BUDDY_GROUP_SIZE);
	k_memset(k_group, 0, K_BUDDY_GROUP_SIZE);

	k_node = (void *)K_ALIGN_UP((k_uint32_t)k_group + K_BUDDY_GROUP_SIZE, 0x1000);
	k_paging_reserve_pages((k_uint32_t)k_node, K_BUDDY_LIST_SIZE);
	k_memset(k_node, 0, K_BUDDY_LIST_SIZE);

	k_heap = (void *)K_ALIGN_UP((k_uint32_t)k_node + K_BUDDY_LIST_SIZE,
			1 << K_BUDDY_MIN_BLOCK_LOG2);
	k_paging_reserve_pages((k_uint32_t)k_heap, 1 << K_BUDDY_MAX_BLOCK_LOG2);

	k_node[0].status = K_BUDDY_NODE_SPLIT;
	k_node[0].order = K_BUDDY_MAX_BLOCK_LOG2;
	k_node[0].next = k_node[0].prev = &k_group[0];

	k_group[0].status = K_BUDDY_NODE_HEAD;
	k_group[0].order = K_BUDDY_MAX_BLOCK_LOG2;
	k_group[0].next = k_group[0].prev = &k_node[0];

	for (i = 1; i < K_BUDDY_TOTAL_GROUPS; i++) {
		k_group[i].status = K_BUDDY_NODE_HEAD;
		k_group[i].order = K_BUDDY_MAX_BLOCK_LOG2 - i;
		k_group[i].next = k_group[i].prev = &k_group[i];
	}
}

