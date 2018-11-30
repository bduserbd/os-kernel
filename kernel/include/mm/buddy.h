#ifndef K_BUDDY_H
#define K_BUDDY_H

#include "kernel/include/error.h"
#include "kernel/include/types.h"

#define K_BUDDY_MIN_BLOCK_LOG2	12
#define K_BUDDY_MAX_BLOCK_LOG2	27

/* Of minimum size. */
#define K_BUDDY_TOTAL_BLOCKS	\
	(1 << (K_BUDDY_MAX_BLOCK_LOG2 - K_BUDDY_MIN_BLOCK_LOG2))

#define K_BUDDY_LIST_SIZE	\
	(sizeof(struct k_buddy_node) * K_BUDDY_TOTAL_BLOCKS)

#define K_BUDDY_TOTAL_GROUPS	\
	(1 + K_BUDDY_MAX_BLOCK_LOG2 - K_BUDDY_MIN_BLOCK_LOG2)

#define K_BUDDY_GROUP_SIZE	\
	(sizeof(struct k_buddy_node) * K_BUDDY_TOTAL_GROUPS)

enum {
	K_BUDDY_NODE_USED,
	K_BUDDY_NODE_SPLIT,
	K_BUDDY_NODE_HEAD,
};

struct k_buddy_node {
	k_int8_t status;
	k_uint8_t order;
	struct k_buddy_node *next, *prev;
} __attribute__((packed));

extern k_uint8_t *k_heap;

void k_buddy_init(k_uint32_t);

void *k_buddy_alloc(k_size_t);
void k_buddy_free(const void *);

void *k_buddy_user_alloc(k_size_t);
void k_buddy_user_free(const void *);

#endif

