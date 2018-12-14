#ifndef K_PAGING_H
#define K_PAGING_H

#ifndef __ASSEMBLER__

#include "kernel/include/types.h"
#include "kernel/include/mm/zone.h"

extern unsigned long k_total_normal_frames;
extern struct k_frame *k_normal_frames;

#define K_FRAME_ARRAY_SIZE	(k_total_normal_frames * sizeof(struct k_frame))

#define K_PHYSICAL_ADDRESS(virtual)	\
	((unsigned long)(virtual) - K_IMAGE_BASE)

#define K_VALUE_PHYSICAL_ADDRESS(virtual)	\
	(*(unsigned long *)K_PHYSICAL_ADDRESS(virtual))

#define K_VIRTUAL_ADDRESS(physical)	\
	((unsigned long)(physical) | K_IMAGE_BASE)

void k_paging_init(void);
void k_paging_table_set_start(k_uint32_t);
void k_paging_reserve_pages(k_uint32_t, k_uint32_t);
void k_paging_build_frame_array(unsigned long);

#else

#define K_PHYSICAL_ADDRESS(virtual)	((virtual) - K_IMAGE_BASE)
#define K_VIRTUAL_ADDRESS(physical)	((physical) + K_IMAGE_BASE)

#endif

/* Page reservation flags. */
#define K_PAGING_RESERVE_HIGH_MEMORY	(1 << 0)
#define K_PAGING_RESERVE_DMA		(1 << 1)
#define K_PAGING_RESERVE_USER		(1 << 2)
#define K_PAGING_RESERVE_THIS_ZONE	(1 << 3)

#endif

