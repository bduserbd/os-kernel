#ifndef K_SLAB_H
#define K_SLAB_H

#include "kernel/include/mm/buddy.h"

#define K_MALLOC_MIN_SIZE_LOG2	3
#define K_MALLOC_MAX_SIZE_LOG2	16

#define K_SLAB_MIN_ALIGNMENT	sizeof(unsigned long)
#define K_SLAB_MAX_ALIGNMENT	(1 << 6)

#define K_SLAB_FLAGS_L1_ALIGNMENT	0x1
#define K_SLAB_FLAGS_MANAGEMENT_OFF	0x2

#define K_SLAB_MANAGEMENT_BLOCK_LOG2	3

typedef k_uint8_t	k_cache_free_object_t;

#define K_SLAB_OBJECT_LIST_END	0xff

#define K_SLAB_MAX_OBJECTS	((sizeof(k_cache_free_object_t) << 8) - 1)

struct k_slab;

struct k_cache {
	const char *name;

	unsigned int flags;

	unsigned int blocks_log2;

	unsigned int object_size;
	unsigned int objects;

	unsigned int alignment;

	unsigned int colour;
	unsigned int colour_off;
	unsigned int colour_next;

	struct k_slab *full_slabs;
	struct k_slab *partial_slabs;
	struct k_slab *free_slabs;
};

struct k_slab {
	void *memory;

	unsigned int active;

	struct k_slab *next;

	k_cache_free_object_t free;
	k_cache_free_object_t free_objects[0];
};

#endif

