#ifndef K_PAGING_H
#define K_PAGING_H

#include "include/types.h"

/* Page Directory Entry (4-KByte). */
#define K_PDE_P		(1 << 0)
#define K_PDE_RW	(1 << 1)
#define K_PDE_US	(1 << 2)
#define K_PDE_PWT	(1 << 3)
#define K_PDE_PCD	(1 << 4)
#define K_PDE_A		(1 << 5)
#define K_PDE_PS	(1 << 7)

typedef k_uint32_t	k_pde_t;

#define K_PD_SIZE	sizeof(k_pde_t) * 0x400

/* Page Table Entry. */
#define K_PTE_P		(1 << 0)
#define K_PTE_RW	(1 << 1)
#define K_PTE_US	(1 << 2)
#define K_PTE_PWT	(1 << 3)
#define K_PTE_PCD	(1 << 4)
#define K_PTE_A		(1 << 5)
#define K_PTE_D		(1 << 6)

typedef k_uint32_t	k_pte_t;

#define K_PT_SIZE	sizeof(k_pte_t) * 0x400

#endif

