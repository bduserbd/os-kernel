#include "include/paging.h"
#include "include/cr0.h"
#include "include/cpu.h"
#include "kernel/include/video/print.h"
#include "kernel/include/string.h"

k_pde_t *k_page_table = NULL;

unsigned long k_total_frames = 0;
struct k_frame *k_frames = NULL;

void *k_p2v(const void *physical)
{
	unsigned long frame, offset;

	offset = (unsigned long)physical & 0xfff;
	frame = (unsigned long)physical >> 12;

	if (frame > k_total_frames)
		return NULL;

	return (void *)((unsigned long)k_frames[frame].virtual + offset);
}

unsigned long k_p2v_l(unsigned long physical)
{
	return (unsigned long)k_p2v((const void *)physical);
}

void k_paging_map_dma(unsigned long physical)
{

}

void k_paging_build_frame_array(unsigned long total_frames)
{
	int i, j;
	struct k_frame *frames;

	k_total_frames = total_frames;
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

	k_frames = frames;
}

static inline void k_paging_flush_tlb(void)
{
	k_uint32_t reg;

	asm volatile("mov %%cr3, %0" : "=r" (reg));
	asm volatile("mov %0, %%cr3" : : "r" (reg));
}

void k_paging_remove_identity_map(void)
{
	k_uint32_t a, b;

	k_page_table = (k_pde_t *)K_VIRTUAL_ADDRESS(k_page_table);

	a = 0x0;
	b = K_IMAGE_BASE - K_MB(4);

	while (1) {
		k_uint32_t table = (a >> 22) & 0x3ff;

		if (k_page_table[table] & K_PTE_P) {
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

void k_paging_reserve_pages_ptr(k_pde_t *k_pde, k_uint32_t start, k_uint32_t range,
		int high_memory)
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

		if ((pde[table] & K_PTE_P) == 0) {
			if (high_memory) {
				pde[table] = ((k_uint32_t)k_pde - K_IMAGE_BASE + 
						0x1000 + table * 0x1000) | K_PDE_P | K_PDE_RW;

				k_memset((void *)(K_IMAGE_BASE + (pde[table] & ~0xfff)), 0, 0x1000);
			} else {
				pde[table] = ((k_uint32_t)k_pde + 0x1000 + table * 0x1000) |
					K_PDE_P | K_PDE_RW;

				k_memset((void *)(pde[table] & ~0xfff), 0, 0x1000);
			}
		}

		if (high_memory) {
			physical = a - K_IMAGE_BASE;
			pte = (k_pte_t *)(K_IMAGE_BASE + (pde[table] & ~0xfff));

			if (k_frames)
				k_frames[physical >> 12].virtual = (void *)((table << 22) + (page << 12));
		} else {
			physical = a;
			pte = (k_pte_t *)((pde[table] & ~0xfff));
		}

		pte[page] = (k_uint32_t)physical | K_PTE_P | K_PTE_RW;

		if (a == b)
			break;

		a += 0x1000;
	}
}

void k_paging_reserve_pages(k_uint32_t start, k_uint32_t range)
{
	k_paging_reserve_pages_ptr(k_page_table, start, range, true);
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

		if (pde[table1] & K_PTE_P) {
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

		k_paging_reserve_pages_ptr((void *)*pde, start, K_PAGE_TABLE_TOTAL_SIZE, false);
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

