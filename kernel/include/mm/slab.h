#ifndef K_SLAB_H
#define K_SLAB_H

#include "kernel/include/types.h"

#define K_SLAB_MIN_OBJECTS	(1 << 3)

#define K_SLAB_MIN_ALIGNMENT	sizeof(unsigned long)

#define K_SLAB_FLAGS_L1_ALIGNMENT	0x1

typedef k_uint8_t	k_cache_free_object_t;

#define K_SLAB_OBJECT_LIST_END	0xff

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

	struct k_slab *next;

	k_cache_free_object_t free;
	k_cache_free_object_t free_objects[0];
};

#endif

