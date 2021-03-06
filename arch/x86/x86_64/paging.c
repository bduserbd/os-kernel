#include "include/x86_64/paging.h"
#include "kernel/include/string.h"
#include "kernel/include/modules/export-symbol.h"
#include "kernel/include/mm/mm.h"

k_pml4e_t *k_page_table = NULL;

unsigned long k_total_normal_frames = 0;
struct k_frame *k_normal_frames = NULL;

/* Right now we use only one PDE - 1GB for kernel. */

static k_pde_t *k_paging_get_pde(unsigned long address)
{
	unsigned long i;
	k_pdpe_t *pdpe;

	i = K_PML4E_INDEX(address);
	if (!(k_page_table[i] & K_PML4E_P))
		return NULL;

	pdpe = (k_pdpe_t *)(K_IMAGE_BASE + (k_page_table[i] & K_PAGE_MASK));
	i = K_PDPE_INDEX(address);
	if (!(pdpe[i] & K_PDPE_P))
		return NULL;

	return (k_pde_t *)(K_IMAGE_BASE + (pdpe[i] & K_PAGE_MASK));
}

void *k_v2p(const void *virtual)
{
	k_pde_t *pde;
	k_pte_t *pte;
	unsigned long a, i;

	pde = k_paging_get_pde(K_IMAGE_BASE);
	if (!pde)
		return NULL;

	a = (unsigned long)virtual;

	i = K_PDE_INDEX(a);
	if (pde[i] & K_PDE_P) {
		pte = (k_pte_t *)(K_IMAGE_BASE + (pde[i] & K_PAGE_MASK));
		i = K_PTE_INDEX(a);
		if (pte[i] & K_PTE_P)
			return (void *)(((pte[i] & K_PAGE_MASK) + (a & K_PAGE_BITS)) & ~K_PTE_NX);
	}

	return NULL;
}
K_EXPORT_FUNC(k_v2p);

unsigned long k_v2p_l(unsigned long virtual)
{
	return (unsigned long)k_v2p((const void *)virtual);
}
K_EXPORT_FUNC(k_v2p_l);

void k_paging_build_frame_array(unsigned long total_frames)
{
	int i, j;
	struct k_frame *frames;
	k_pde_t *pde;

	k_total_normal_frames = total_frames;
	frames = (struct k_frame *)K_OFFSET_FROM(k_page_table, K_PAGE_TABLE_TOTAL_SIZE);

	k_paging_reserve_pages((unsigned long)frames, K_FRAME_ARRAY_SIZE);

	k_memset(frames, 0, K_FRAME_ARRAY_SIZE);

	pde = k_paging_get_pde(K_IMAGE_BASE);
	if (!pde)
		return;

	for (i = K_PDE_INDEX(K_IMAGE_BASE); i < K_PAGE_TABLE_ENTRIES; i++) {
		if (!(pde[i] & K_PDE_P))
			continue;

		k_pte_t *pte = (k_pte_t *)(K_IMAGE_BASE + (pde[i] & K_PAGE_MASK));
		for (j = 0; j < K_PAGE_TABLE_ENTRIES; j++) {
			if (!(pte[j] & K_PTE_P))
				continue;

			frames[K_PAGE_NUMBER(pte[j] & ~K_PTE_NX)].virtual =
				(void *)K_PAGE_TABLES_TO_ADDRESS(K_PML4E_INDEX(K_IMAGE_BASE),
						K_PDPE_INDEX(K_IMAGE_BASE), i, j, 0x0);
		}
	}

	k_normal_frames = frames;
}

static inline void k_paging_set_nx(k_pte_t *pte)
{
	*pte |= K_PTE_NX;
}

static void k_paging_reserve_pages_ptr(unsigned long start, unsigned long range,
		unsigned long dma, unsigned int flags)
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
		k_pde_t *pde = k_paging_get_pde(a);
		if (!pde)
			return;

		i = K_PDE_INDEX(a);
		if (!(pde[i] & K_PDE_P)) {
			pde[i] = ((unsigned long)pde - K_IMAGE_BASE + K_PAGE_TABLE_SIZE +
					K_PDE_INDEX(a) * K_PAGE_TABLE_SIZE) |
				K_PDE_P | K_PDE_RW;

			if (flags & K_PAGING_RESERVE_USER)
				pde[i] |= K_PDE_US;
		}

		k_pte_t *pte = (k_pte_t *)(K_IMAGE_BASE + (pde[i] & K_PAGE_MASK));
		i = K_PTE_INDEX(a);
		if (!(pte[i] & K_PTE_P)) {
			unsigned long physical;

			if (flags & K_PAGING_RESERVE_DMA)
				physical = dma + a - (start & K_PAGE_MASK);
			else
				physical = a - K_IMAGE_BASE;

			pte[i] = physical | K_PTE_P | K_PTE_RW;

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

void k_paging_reserve_dma(unsigned long start, unsigned long dma, unsigned long range)
{
	k_paging_reserve_pages_ptr(start, range, dma, K_PAGING_RESERVE_DMA);
	k_paging_flush_tlb();
}

static k_pdpe_t *ap_pdpe = NULL;
static k_pde_t *ap_pde = NULL;
static k_pte_t *ap_pte = NULL;

void k_paging_map_ap_start(unsigned long address)
{
	unsigned long i;

	/* PML4E */
	ap_pdpe = k_zalloc(K_PAGE_TABLE_ENTRIES * sizeof(k_pdpe_t));
	if (!ap_pdpe)
		return;

	i = K_PML4E_INDEX(address);
	k_page_table[i] = ((unsigned long)ap_pdpe - K_IMAGE_BASE) |
		K_PML4E_P | K_PML4E_RW;

	/* PDPE */
	ap_pde = k_zalloc(K_PAGE_TABLE_ENTRIES * sizeof(k_pde_t));
	if (!ap_pde)
		return;

	i = K_PDPE_INDEX(address);
	ap_pdpe[i] = ((unsigned long)ap_pde - K_IMAGE_BASE) |
		K_PDPE_P | K_PDPE_RW;


	/* PDE */
	ap_pte = k_zalloc(K_PAGE_TABLE_ENTRIES * sizeof(k_pte_t));
	if (!ap_pte)
		return;

	i = K_PDE_INDEX(address);
	ap_pde[i] = ((unsigned long)ap_pte - K_IMAGE_BASE) |
		K_PDE_P | K_PDE_RW;

	/* PTE */
	i = K_PTE_INDEX(address);
	ap_pte[i] = (address & K_PAGE_MASK) | K_PTE_P | K_PTE_RW;
	
	k_paging_flush_tlb();
}

void k_paging_reserve_pages(unsigned long start, unsigned long range)
{
	k_paging_reserve_pages_ptr(start, range, 0, 0);
	k_paging_flush_tlb();
}

void k_paging_reserve_execute_pages(unsigned long start, unsigned long range)
{
	k_paging_reserve_pages_ptr(start, range, 0, K_PAGING_RESERVE_EXECUTE);
	k_paging_flush_tlb();
}

extern __u8 __k_start[];
extern __u8 __k_end[];

void k_paging_arch_init(void)
{
	k_paging_reserve_execute_pages((unsigned long)__k_start, (unsigned long)(__k_end -
				__k_start));

	k_paging_reserve_pages((unsigned long)k_page_table, K_PAGE_TABLE_TOTAL_SIZE);
}

