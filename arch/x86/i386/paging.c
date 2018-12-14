#include "include/paging.h"
#include "include/i386/paging.h"
#include "include/cr0.h"
#include "include/cpu.h"
#include "kernel/include/string.h"
#include "kernel/include/modules/export-symbol.h"
#include "kernel/include/video/print.h"

k_pde_t *k_page_table = NULL;

unsigned long k_total_normal_frames = 0;
struct k_frame *k_normal_frames = NULL;

static inline void k_paging_flush_tlb(void)
{
	k_uint32_t reg;

	asm volatile("mov %%cr3, %0" : "=r" (reg));
	asm volatile("mov %0, %%cr3" : : "r" (reg));
}

void k_paging_build_frame_array(unsigned long total_frames)
{
	int i, j;
	struct k_frame *frames;

	k_total_normal_frames = total_frames;
	frames = (struct k_frame *)K_OFFSET_FROM(k_page_table, K_PAGE_TABLE_TOTAL_SIZE);

	k_paging_reserve_pages((unsigned long)frames, K_FRAME_ARRAY_SIZE);

	k_memset(frames, 0, K_FRAME_ARRAY_SIZE);

	for (i = (K_IMAGE_BASE >> 22); i < 1024; i++) {
		if (!(k_page_table[i] & K_PDE_P))
			continue;

		k_pte_t *pte = (k_pte_t *)(K_IMAGE_BASE + (k_page_table[i] & ~0xfff));
		for (j = 0; j < 1024; j++) {
			if (!(pte[j] & K_PTE_P))
				continue;

			frames[pte[j] >> 12].virtual = (void *)((i << 22) + (j << 12));
		}
	}

	k_normal_frames = frames;
}

void k_paging_remove_identity_map(void)
{
	k_uint32_t a, b;

	k_page_table = (k_pde_t *)K_VIRTUAL_ADDRESS(k_page_table);

	a = 0x0;
	b = K_IMAGE_BASE - K_MB(4);

	while (1) {
		k_uint32_t table = (a >> 22) & 0x3ff;

		if (k_page_table[table] & K_PDE_P) {
			k_uint32_t pde = k_page_table[table];
			k_page_table[table] = 0x0;
			k_memset((void *)(K_IMAGE_BASE + (pde & ~0xfff)), 0, 0x1000);
		}

		if (a == b)
			break;

		a += K_MB(4);
	}

	k_paging_flush_tlb();
}

void *k_v2p(const void *virtual)
{
	k_pte_t *pte;
	unsigned long table, page, offset;

	table = ((unsigned long)virtual >> 22) & 0x3ff;
	page = ((unsigned long)virtual >> 12) & 0x3ff;
	offset = (unsigned long)virtual & 0xfff;

	if (k_page_table[table] & K_PDE_P) {
		pte = (k_pte_t *)(K_IMAGE_BASE + (k_page_table[table] & ~0xfff));
		if (pte[page] & K_PTE_P)
			return (void *)((pte[page] & ~0xfff) + offset);
	}

	return NULL;
}
K_EXPORT_FUNC(k_v2p);

unsigned long k_v2p_l(unsigned long virtual)
{
	return (unsigned long)k_v2p((const void *)virtual);
}
K_EXPORT_FUNC(k_v2p_l);

void k_paging_reserve_pages_ptr(k_pde_t *k_pde, k_uint32_t start, k_uint32_t range,
		unsigned long dma, unsigned int flags)
{
	k_uint32_t a, b;
	k_uint32_t physical;
	k_pde_t *pde;
	k_pte_t *pte;

	if (!k_pde)
		return;

	a = start & ~0xfff;

	b = start + range;
	if (b & 0xfff)
		b &= ~0xfff;
	else
		b -= 0x1000;

	pde = k_pde;

	while (1) {
		k_uint32_t table = (a >> 22) & 0x3ff;
		k_uint32_t page = (a >> 12) & 0x3ff;

		if ((pde[table] & K_PDE_P) == 0) {
			unsigned int pde_flags = K_PDE_P | K_PDE_RW;
			if (flags & K_PAGING_RESERVE_USER)
				pde_flags |= K_PDE_US;

			if (flags & K_PAGING_RESERVE_HIGH_MEMORY) {
				pde[table] = ((k_uint32_t)k_pde - K_IMAGE_BASE + 
						0x1000 + table * 0x1000) | pde_flags;

				k_memset((void *)(K_IMAGE_BASE + (pde[table] & ~0xfff)), 0, 0x1000);
			} else {
				pde[table] = ((k_uint32_t)k_pde + 0x1000 + table * 0x1000) | pde_flags;

				k_memset((void *)(pde[table] & ~0xfff), 0, 0x1000);
			}
		}

		if (flags & K_PAGING_RESERVE_HIGH_MEMORY) {
			pte = (k_pte_t *)(K_IMAGE_BASE + (pde[table] & ~0xfff));

			if (flags & K_PAGING_RESERVE_DMA)
				physical = dma + a - (start & ~0xfff);
			else
				physical = a - K_IMAGE_BASE;

			if (flags & K_PAGING_RESERVE_THIS_ZONE)
				if (k_normal_frames)
					k_normal_frames[physical >> 12].virtual =
						(void *)((table << 22) + (page << 12));
		} else {
			physical = a;
			pte = (k_pte_t *)((pde[table] & ~0xfff));
		}

		pte[page] = (k_uint32_t)physical | K_PTE_P | K_PTE_RW;
		if (flags & K_PAGING_RESERVE_USER)
			pte[page] |= K_PTE_US;

		if (a == b)
			break;

		a += 0x1000;
	}
}

void k_paging_map_dma(unsigned long virtual, unsigned long physical, unsigned long range)
{
	k_paging_reserve_pages_ptr(k_page_table, virtual, range, physical,
					K_PAGING_RESERVE_HIGH_MEMORY |
					K_PAGING_RESERVE_DMA);

	k_paging_flush_tlb();
}

void k_paging_map_user(unsigned virtual, unsigned physical, unsigned long range)
{
	k_paging_reserve_pages_ptr(k_page_table, virtual, range, physical,
					K_PAGING_RESERVE_HIGH_MEMORY |
					K_PAGING_RESERVE_DMA |
					K_PAGING_RESERVE_USER |
					K_PAGING_RESERVE_THIS_ZONE);

	k_paging_flush_tlb();
}

void k_paging_reserve_pages(k_uint32_t start, k_uint32_t range)
{
	k_paging_reserve_pages_ptr(k_page_table, start, range, 0,
					K_PAGING_RESERVE_HIGH_MEMORY |
					K_PAGING_RESERVE_THIS_ZONE);

	k_paging_flush_tlb();
}

void k_paging_identity_map(void)
{
	k_uint32_t a, b;
	k_pde_t *pde;

	pde = (k_pde_t *)K_VALUE_PHYSICAL_ADDRESS(&k_page_table);

	a = 0x0;
	b = K_IMAGE_BASE - K_MB(4);

	while (1) {
		k_uint32_t table1 = (a >> 22) & 0x3ff;

		if (pde[table1] & K_PDE_P) {
			k_uint32_t table2 = table1 + (K_IMAGE_BASE >> 22);

			pde[table2] = ((k_uint32_t)pde + 0x1000 + table2 * 0x1000) |
				K_PDE_P | K_PDE_RW;

			k_memcpy((void *)(pde[table2] & ~0xfff),
					(void *)(pde[table1] & ~0xfff), 0x1000);
		}

		if (a == b)
			break;

		a += K_MB(4);
	}
}

void k_paging_table_set_start(k_uint32_t start)
{
	unsigned long *pde;

	pde = (unsigned long *)K_PHYSICAL_ADDRESS(&k_page_table);

	if (!*pde) {
		if (start & 0xfff)
			return;

		*pde = start;

		k_memset((void *)*pde, 0, 0x1000);

		k_paging_reserve_pages_ptr((void *)*pde, start, K_PAGE_TABLE_TOTAL_SIZE,
				0, 0);
	}
}

void k_paging_init(void)
{
	unsigned long k_pde;
	k_uint32_t cr0;

	k_pde = K_VALUE_PHYSICAL_ADDRESS(&k_page_table);

	asm volatile("mov %0, %%cr3" : : "r" (k_pde));

	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80010000;

	asm volatile("mov %0, %%cr0" : : "r" (cr0));
}

