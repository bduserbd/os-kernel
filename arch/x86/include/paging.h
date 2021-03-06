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

#define K_PAGE_NUMBER(address)	((address) >> 12)

void k_paging_init(void);
void k_paging_reserve_pages(unsigned long, unsigned long);
void k_paging_reserve_execute_pages(unsigned long, unsigned long);
void k_paging_build_frame_array(unsigned long);

static inline void k_paging_flush_tlb(void)
{
	unsigned long cr3;

	asm volatile("mov %%cr3, %0" : "=r" (cr3));
	asm volatile("mov %0, %%cr3" : : "r" (cr3));
}

static inline unsigned long k_paging_fault_address(void)
{
	unsigned long cr2;

	asm volatile("mov %%cr2, %0" : "=r" (cr2));

	return cr2;
}

#else

#define K_PHYSICAL_ADDRESS(virtual)	((virtual) - K_IMAGE_BASE)
#define K_VIRTUAL_ADDRESS(physical)	((physical) + K_IMAGE_BASE)

#endif

/* Page reservation flags. */
#define K_PAGING_RESERVE_HIGH_MEMORY	(1 << 0)
#define K_PAGING_RESERVE_DMA		(1 << 1)
#define K_PAGING_RESERVE_USER		(1 << 2)
#define K_PAGING_RESERVE_THIS_ZONE	(1 << 3)
#define K_PAGING_RESERVE_EXECUTE	(1 << 4)

#endif

