#ifndef K_PAGING_H
#define K_PAGING_H

#ifndef __ASSEMBLER__

#include "kernel/include/types.h"

typedef k_uint32_t	k_pde_t;
typedef k_uint32_t	k_pte_t;

#define K_PHYSICAL_ADDRESS(virtual)	\
	((unsigned long)(virtual) - K_IMAGE_BASE)

#define K_VALUE_PHYSICAL_ADDRESS(virtual)	\
	(*(unsigned long *)K_PHYSICAL_ADDRESS(virtual))

#define K_VIRTUAL_ADDRESS(physical)	\
	((unsigned long)(physical) + K_IMAGE_BASE)

extern k_pde_t *k_page_table;

void k_paging_init(void);
void k_paging_table_set_start(k_uint32_t);
void k_paging_reserve_pages(k_uint32_t, k_uint32_t);

#else

#define K_PHYSICAL_ADDRESS(virtual)	((virtual) - K_IMAGE_BASE)
#define K_VIRTUAL_ADDRESS(physical)	((physical) + K_IMAGE_BASE)

#endif

/* Page Directory Entry (4-KByte). */
#define K_PDE_P		(1 << 0)
#define K_PDE_RW	(1 << 1)
#define K_PDE_US	(1 << 2)
#define K_PDE_PWT	(1 << 3)
#define K_PDE_PCD	(1 << 4)
#define K_PDE_A		(1 << 5)
#define K_PDE_PS	(1 << 7)

/* Page Table Entry. */
#define K_PTE_P		(1 << 0)
#define K_PTE_RW	(1 << 1)
#define K_PTE_US	(1 << 2)
#define K_PTE_PWT	(1 << 3)
#define K_PTE_PCD	(1 << 4)
#define K_PTE_A		(1 << 5)
#define K_PTE_D		(1 << 6)

#define K_PAGE_TABLE_MAP_SIZE           (0x400 << 12)

#endif

