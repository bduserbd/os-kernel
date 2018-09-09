#ifndef K_PAGING_H
#define K_PAGING_H

/* Page Directory Entry (4-KByte). */
#define K_PDE_P		(1 << 0)
#define K_PDE_RW	(1 << 1)
#define K_PDE_US	(1 << 2)
#define K_PDE_PWT	(1 << 3)
#define K_PDE_PCD	(1 << 4)
#define K_PDE_A		(1 << 5)
#define K_PDE_PS	(1 << 7)

typedef k_uint32_t	pde_t;

/* Page Table Entry. */
#define K_PTE_P		(1 << 0)
#define K_PTE_RW	(1 << 1)
#define K_PTE_US	(1 << 2)
#define K_PTE_PWT	(1 << 3)
#define K_PTE_PCD	(1 << 4)
#define K_PTE_A		(1 << 5)
#define K_PTE_D		(1 << 6)

typedef k_uint32_t	pte_t;

#endif

