#include "include/x86_64/paging.h"

k_pml4e_t *k_page_table = NULL;

void k_paging_set_nx(k_pte_t *pte)
{
	*pte |= K_PTE_NX;
}

static void k_paging_reserve_pages_ptr(unsigned long start, unsigned long range,
		unsigned int flags)
{
	unsigned long a, b;
	unsigned long i;

	a = start & K_PAGE_MASK;

	b = start + range;
	if (b & K_PAGE_BITS)
		b &= K_PAGE_MASK;
	else
		b -= K_PAGE_SIZE;

	while (1) {
		i = K_PML4E_INDEX(a);
		if (!(k_page_table[i] & K_PML4E_P))
			return;

		k_pdpe_t *pdpe = (k_pdpe_t *)(K_IMAGE_BASE + (k_page_table[i] & K_PAGE_MASK));
		i = K_PDPE_INDEX(a);
		if (!(pdpe[i] & K_PDPE_P))
			return;

		k_pde_t *pde = (k_pde_t *)(K_IMAGE_BASE + (pdpe[i] & K_PAGE_MASK));
		i = K_PDE_INDEX(a);
		if (!(pde[i] & K_PDE_P)) {
			pde[i] = ((unsigned long)pde + K_PAGE_TABLE_SIZE + K_PTE_INDEX(a) *
						K_PAGE_TABLE_SIZE) | K_PDE_P | K_PDE_RW;

			if (flags & K_PAGING_RESERVE_USER)
				pde[i] |= K_PDE_US;
		}

		k_pte_t *pte = (k_pte_t *)(K_IMAGE_BASE + (pde[i] & K_PAGE_MASK));
		i = K_PTE_INDEX(a);
		if (!(pte[i] & K_PTE_P)) {
			pte[i] = (unsigned long)(a - K_IMAGE_BASE) | K_PTE_P | K_PTE_RW;

			if (flags & K_PAGING_RESERVE_USER)
				pte[i] |= K_PTE_US;
		}

		if (!(flags & K_PAGING_RESERVE_EXECUTE))
			k_paging_set_nx(&pte[i]);

		if (a == b)
			break;

		a += K_PAGE_SIZE;
	}
}

extern __u8 __k_start[];
extern __u8 __k_end[];

void k_paging_reserve_pages(unsigned long start, unsigned long range)
{
	k_paging_reserve_pages_ptr(start, range, 0);
	k_paging_flush_tlb();
}

void k_paging_reserve_execute_pages(unsigned long start, unsigned long range)
{
	k_paging_reserve_pages_ptr(start, range, K_PAGING_RESERVE_EXECUTE);
	k_paging_flush_tlb();
}

void k_paging_arch_init(void)
{
	k_paging_reserve_execute_pages((unsigned long)__k_start, (unsigned long)(__k_end -
			__k_start));

	k_paging_reserve_pages((unsigned long)k_page_table, K_PAGE_TABLE_TOTAL_SIZE);
}

