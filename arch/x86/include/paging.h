#ifndef PAGING_H
#define PAGING_H

/* Page Directory Entry. */
#define PDE_P	(1 << 0)
#define PDE_R	(1 << 1)
#define PDE_U	(1 << 2)
#define PDE_W	(1 << 3)
#define PDE_D	(1 << 4)
#define PDE_A	(1 << 5)
#define PDE_S	(1 << 7)

typedef unsigned int	pde_t;

/* Page Table Entry. */
#define PTE_P	(1 << 0)
#define PTE_R	(1 << 1)
#define PTE_U	(1 << 2)
#define PTE_W	(1 << 3)
#define PTE_C	(1 << 4)
#define PTE_A	(1 << 5)
#define PTE_D	(1 << 6)

typedef unsigned int	pte_t;

#endif

