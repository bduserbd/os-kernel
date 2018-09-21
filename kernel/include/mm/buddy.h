#ifndef K_BUDDY_SYSTEM_H
#define K_BUDDY_SYSTEM_H

#include "kernel/include/error.h"
#include "kernel/include/types.h"

#define K_BUDDY_SYSTEM_MIN_BLOCK_LOG2	12
#define K_BUDDY_SYSTEM_MAX_BLOCK_LOG2	27

/* Of minimum size. */
#define K_BUDDY_SYSTEM_TOTAL_BLOCKS	\
	(1 << (K_BUDDY_SYSTEM_MAX_BLOCK_LOG2 - K_BUDDY_SYSTEM_MIN_BLOCK_LOG2))

#define K_BUDDY_SYSTEM_LIST_SIZE	\
	(sizeof(struct k_buddy_node) * K_BUDDY_SYSTEM_TOTAL_BLOCKS)

struct k_buddy_node {
	k_uint8_t order;
	k_int8_t free;
	struct k_buddy_node *next;
} __attribute__((packed));

struct k_buddy_list {
	k_uint32_t free_blocks;
	struct k_buddy_node *node;
};

void k_buddy_init(k_uint32_t);

#endif

