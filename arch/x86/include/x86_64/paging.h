#ifndef K_X86_64_PAGING_H
#define K_X86_64_PAGING_H

#include "arch/x86/include/paging.h"

#ifndef __ASSEMBLER__

#include "kernel/include/types.h"

typedef k_uint64_t	k_pml4e_t;
typedef k_uint64_t	k_pdpe_t;
typedef k_uint64_t	k_pde_t;
typedef k_uint64_t	k_pte_t;

#endif

/* Page map level 4 entry. */
#define K_PML4E_P	(1 << 0)
#define K_PML4E_RW	(1 << 1)
#define K_PML4E_US	(1 << 2)
#define K_PML4E_PWT	(1 << 3)
#define K_PML4E_PCD	(1 << 4)
#define K_PML4E_A	(1 << 5)

/* Page directory pointer table. */
#define K_PDPE_P	(1 << 0)
#define K_PDPE_RW	(1 << 1)
#define K_PDPE_US	(1 << 2)
#define K_PDPE_PWT	(1 << 3)
#define K_PDPE_PCD	(1 << 4)
#define K_PDPE_A	(1 << 5)

/* Page directroy entry. */
#define K_PDE_P		(1 << 0)
#define K_PDE_RW	(1 << 1)
#define K_PDE_US	(1 << 2)
#define K_PDE_PWT	(1 << 3)
#define K_PDE_PCD	(1 << 4)
#define K_PDE_A		(1 << 5)

/* Page table entry. */
#define K_PTE_P		(1 << 0)
#define K_PTE_RW	(1 << 1)
#define K_PTE_US	(1 << 2)
#define K_PTE_PWT	(1 << 3)
#define K_PTE_PCD	(1 << 4)
#define K_PTE_A		(1 << 5)

#define K_PAGE_TABLE_ENTRIES	512
#define K_PAGE_TABLE_SIZE	0x1000
#define K_PAGE_TABLE_TOTAL_SIZE	(0x1000 + 0x1000 + 0x1000 + (512 * 0x1000))

#define K_PML4E_INDEX(address)	(((address) >> 39ULL) & 0x1ff)
#define K_PDPE_INDEX(address)	(((address) >> 30ULL) & 0x1ff)
#define K_PDE_INDEX(address)	(((address) >> 21ULL) & 0x1ff)
#define K_PTE_INDEX(address)	(((address) >> 12ULL) & 0x1ff)

#endif

