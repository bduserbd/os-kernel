#include "include/mm/buddy.h"
#include "include/modules/export-symbol.h"
#include "include/string.h"
#include "include/bits.h"

extern unsigned long k_total_normal_frames;

struct k_buddy {
        k_uint8_t *heap;

        unsigned int total_groups, group_size;

        unsigned long list_size;

        unsigned int min_block_log2;
        unsigned int max_block_log2;

        struct k_buddy_node *group;
        struct k_buddy_node *node;
};

k_uint8_t *k_heap = NULL;

static struct k_buddy k_buddy;

/* Physical page frames. */
static struct k_buddy k_user_buddy;

void k_paging_reserve_pages(unsigned long, unsigned long);
void k_paging_reserve_execute_pages(unsigned long, unsigned long);

static struct k_buddy_node *k_buddy_get(struct k_buddy *buddy, struct k_buddy_node *node)
{
	unsigned long a;

	a = (node - buddy->node) << buddy->min_block_log2;

	return &buddy->node[(a ^ (1 << node->order)) >> buddy->min_block_log2];
}

static int k_buddy_best_fit_group(struct k_buddy *buddy, k_size_t size)
{
	int i;

	for (i = 0; i < buddy->total_groups; i++)
		if (size > (1  << (buddy->max_block_log2 - i - 1)))
			return i;

	return i - 1;
}

static k_uint8_t *k_buddy_node_to_address(struct k_buddy *buddy, struct k_buddy_node *node)
{
	return buddy->heap + ((node - buddy->node) << buddy->min_block_log2);
}

static struct k_buddy_node *k_buddy_address_to_node(struct k_buddy *buddy, const void *ptr)
{
	unsigned long a;

	a = (unsigned long)ptr;

	if (a & ((1 << buddy->min_block_log2) - 1))
		return NULL;

	if (a < (unsigned long)buddy->heap ||
			(unsigned long)buddy->heap + (1 << buddy->max_block_log2) < a)
		return NULL;

	return &buddy->node[(a - (unsigned long)buddy->heap) >> buddy->min_block_log2];
}

void k_buddy_free_generic(struct k_buddy *buddy, const void *ptr)
{
	k_uint8_t order;
	struct k_buddy_node *a, *b;

	if (buddy->group || buddy->node || buddy->heap)
		return;

	a = k_buddy_address_to_node(buddy, ptr);
	if (!a)
		return;

	order = a->order;

	while (1) {
		b = k_buddy_get(buddy, a);

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
	b = buddy->group[order].next;
	a->next = b;
	b->prev = a;
	a->order = order;
	a->prev = &buddy->group[order];
	buddy->group[order].next = a;
}

void k_buddy_free(const void *ptr)
{
	return k_buddy_free_generic(&k_buddy, ptr);
}
K_EXPORT_FUNC(k_buddy_free);

void k_buddy_user_free(const void *ptr)
{
	return k_buddy_free_generic(&k_user_buddy, ptr);
}

static void *k_buddy_alloc_generic(struct k_buddy *buddy, k_size_t size)
{
	unsigned int i, group;
	struct k_buddy_node *a, *b;

	if (!buddy->group || !buddy->node || !buddy->heap)
		return NULL;

	group = k_buddy_best_fit_group(buddy, size);

	i = buddy->total_groups;
	while (1) {
		i--;

		if ((buddy->group[i].next == &buddy->group[i]) || (size > (1 << buddy->group[i].order)))
			continue;

		a = buddy->group[i].next;
		b = a->next;
		buddy->group[i].next = b;
		b->prev = &buddy->group[i];

		a->status = K_BUDDY_NODE_USED;		

		break;
	}

	while (1) {
		if (i == group)
			return k_buddy_node_to_address(buddy, a);

		i++;
		a->order--;

		b = k_buddy_get(buddy, a);
		b->status = K_BUDDY_NODE_SPLIT;
		b->order = a->order;

		b->next = b->prev = &buddy->group[i];
		buddy->group[i].next = buddy->group[i].prev = b;
	}
}

static void *k_buddy_alloc_type(k_size_t size, int executable)
{
	void *ptr;
	unsigned long pages;

	ptr = k_buddy_alloc_generic(&k_buddy, size);
	if (!ptr)
		return NULL;

	pages = 1 << (k_buddy.max_block_log2 - k_buddy_best_fit_group(&k_buddy, size));

	if (executable)
		k_paging_reserve_execute_pages((unsigned long)ptr, pages);
	else
		k_paging_reserve_pages((unsigned long)ptr, pages);

	return ptr;
}

void *k_buddy_alloc(k_size_t size)
{
	return k_buddy_alloc_type(size, false);
}
K_EXPORT_FUNC(k_buddy_alloc);

void *k_buddy_alloc_execute(k_size_t size)
{
	return k_buddy_alloc_type(size, true);
}

void *k_buddy_user_alloc(k_size_t size)
{
	void *ptr;

	ptr = k_buddy_alloc_generic(&k_user_buddy, size);
	if (!ptr)
		return NULL;

	if (k_total_normal_frames <= ((unsigned long)ptr >> k_user_buddy.min_block_log2))
		return NULL;

	return ptr;
}

static void k_buddy_reorder_nodes(struct k_buddy *buddy)
{
	int i;

	buddy->node[0].status = K_BUDDY_NODE_SPLIT;
	buddy->node[0].order = buddy->max_block_log2;
	buddy->node[0].next = buddy->node[0].prev = &buddy->group[0];

	buddy->group[0].status = K_BUDDY_NODE_HEAD;
	buddy->group[0].order = buddy->max_block_log2;
	buddy->group[0].next = buddy->group[0].prev = &buddy->node[0];

	for (i = 1; i < buddy->total_groups; i++) {
		buddy->group[i].status = K_BUDDY_NODE_HEAD;
		buddy->group[i].order = buddy->max_block_log2 - i;
		buddy->group[i].next = buddy->group[i].prev = &buddy->group[i];
	}
}

static void k_buddy_kernel_init(unsigned long heap)
{
	k_buddy.total_groups = K_BUDDY_TOTAL_GROUPS;
	k_buddy.group_size = K_BUDDY_GROUP_SIZE;

	k_buddy.list_size = K_BUDDY_LIST_SIZE;

	k_buddy.min_block_log2 = K_BUDDY_MIN_BLOCK_LOG2;
	k_buddy.max_block_log2 = K_BUDDY_MAX_BLOCK_LOG2;

	k_buddy.group = (void *)heap;
	k_paging_reserve_pages(heap, k_buddy.group_size);
	k_memset(k_buddy.group, 0, k_buddy.group_size);

	k_buddy.node = (void *)K_ALIGN_UP((unsigned long)k_buddy.group + k_buddy.group_size, 0x1000);
	k_paging_reserve_pages((unsigned long)k_buddy.node, k_buddy.list_size);
	k_memset(k_buddy.node, 0, k_buddy.list_size);

	k_buddy.heap = (void *)K_ALIGN_UP((unsigned long)k_buddy.node + k_buddy.list_size,
			1 << k_buddy.min_block_log2);

	k_buddy_reorder_nodes(&k_buddy);

	k_heap = k_buddy.heap;
}

static void k_buddy_user_init(void)
{
	unsigned int log2;
	unsigned long left_frames;

	k_user_buddy.min_block_log2 = k_buddy.min_block_log2;

	left_frames = k_total_normal_frames -
			((unsigned long)(k_buddy.heap - K_IMAGE_BASE) >>
			k_user_buddy.min_block_log2);

	log2 = k_log2(left_frames) + !!(left_frames & (left_frames - 1));

	k_user_buddy.max_block_log2 = log2 + k_user_buddy.min_block_log2;

	k_user_buddy.total_groups = 1 + k_user_buddy.max_block_log2 - k_user_buddy.min_block_log2;
	k_user_buddy.group_size = k_user_buddy.total_groups * sizeof(struct k_buddy_node);

	k_user_buddy.list_size = sizeof(struct k_buddy_node) *
			(1 << (k_user_buddy.max_block_log2 - k_user_buddy.min_block_log2));

	k_user_buddy.group = k_buddy_alloc(k_user_buddy.group_size);
	if (!k_user_buddy.group)
		return;

	k_memset(k_user_buddy.group, 0, k_user_buddy.group_size);

	k_user_buddy.node = k_buddy_alloc(k_user_buddy.list_size);
	if (!k_user_buddy.node)
		return;

	k_memset(k_user_buddy.node, 0, k_user_buddy.list_size);

	k_user_buddy.heap = k_buddy.heap + (1 << k_buddy.max_block_log2) - K_IMAGE_BASE;

	k_buddy_reorder_nodes(&k_user_buddy);
}

void k_buddy_init(unsigned long heap)
{
	if (k_buddy.group || k_buddy.node || k_buddy.heap)
		return;

	k_buddy_kernel_init(heap);

	k_buddy_user_init();
}

