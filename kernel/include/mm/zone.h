#ifndef K_ZONE_H
#define K_ZONE_H

#include "kernel/include/types.h"

struct k_frame {
	void *virtual;
};

#define K_MEMORY_ZONE_NORMAL	0
#define K_MEMORY_ZONE_DMA	1

struct k_memory_zone {
	int type;

	unsigned long start_frame;
	unsigned long total_frames;

	struct k_frame *frames;

	struct k_memory_zone *next;
};

void k_memory_zone_init(struct k_frame *, unsigned long, unsigned long);
void k_memory_zone_dma_add(unsigned long, unsigned long);

#endif

