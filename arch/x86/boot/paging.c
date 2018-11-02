#include "include/paging.h"
#include "include/cr0.h"
#include "include/cpu.h"
#include "kernel/include/string.h"

k_pde_t *k_page_table = NULL;

static inline void k_paging_flush_tlb(void)
{
	k_uint32_t reg;

	asm volatile("mov %%cr3, %0" : "=r" (reg));
	asm volatile("mov %0, %%cr3" : : "r" (reg));
}

void k_paging_remove_identity_map(void)
{
	k_uint32_t a, b;
	k_uint32_t pde;

	k_page_table = (k_pde_t *)K_VIRTUAL_ADDRESS(k_page_table);

	a = 0x0;
	b = K_IMAGE_BASE - K_MB(4);

	while (1) {
		k_uint32_t table = (a >> 22) & 0x3ff;

		if (k_page_table[table] & K_PTE_P) {
			//pde = k_page_table[table];
			k_page_table[table] = 0x0;
			//k_memset((void *)(pde & ~0xfff), 0, 0x1000);
		}

		if (a == b)
			break;

		a += K_MB(4);
	}

	k_paging_flush_tlb();
}

static void k_paging_reserve_pages_ptr(k_pde_t *k_pde, k_uint32_t start, k_uint32_t range,
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
			pde[table] = ((k_uint32_t)k_pde + 0x1000 + table * 0x1000) |
				K_PDE_P | K_PDE_RW;

			k_memset((void *)(pde[table] & ~0xfff), 0, 0x1000);
		}

		pte = (k_pte_t *)(pde[table] & ~0xfff);

		if (high_memory)
			physical = a - K_IMAGE_BASE;
		else
			physical = a;

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

void k_paging_identity_map(k_uint32_t virtual, k_uint32_t physical, k_uint32_t range)
{
	unsigned long k_pde;

	k_pde = K_VALUE_PHYSICAL_ADDRESS(&k_page_table);

	k_paging_reserve_pages_ptr((k_pde_t *)k_pde, physical, range, false);
	k_paging_reserve_pages_ptr((k_pde_t *)k_pde, virtual, range, true);
}

void k_paging_table_set_start(k_uint32_t start)
{
	unsigned long *k_pde;

	k_pde = (unsigned long *)K_PHYSICAL_ADDRESS(&k_page_table);

	if (!*k_pde) {
		if (start & 0xfff)
			return;

		*k_pde = start;

		k_memset((void *)*k_pde, 0, 0x1000);

		k_paging_identity_map(start + K_IMAGE_BASE, start, 0x1000 + 0x400 * 0x1000);
	}
}

void k_paging_init(void)
{
	unsigned long k_pde;
	k_uint32_t cr0;

	k_pde = K_VALUE_PHYSICAL_ADDRESS(&k_page_table);

	asm volatile("mov %0, %%cr3" : : "r" (k_pde));

	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= K_CR0_PG;

	asm volatile("mov %0, %%cr0" : : "r" (cr0));
}

