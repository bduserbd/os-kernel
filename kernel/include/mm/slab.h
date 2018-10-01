#ifndef K_SLAB_H
#define K_SLAB_H

#include "kernel/include/types.h"

struct k_cache {
	const char *name;

	unsigned int frames_log2;

	unsigned int obj_size;
};

struct k_slab {
	void *memory;
};

#endif

