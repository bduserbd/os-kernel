#ifndef K_I386_PAGING_H
#define K_I386_PAGING_H

#include "arch/x86/include/paging.h"

#ifndef __ASSEMBLER__

#include "kernel/include/types.h"

typedef k_uint32_t	k_pde_t;
typedef k_uint32_t	k_pte_t;

extern k_pde_t *k_page_table;

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
#define K_PAGE_TABLE_TOTAL_SIZE		(0x1000 + (0x400 * 0x1000))

#endif

