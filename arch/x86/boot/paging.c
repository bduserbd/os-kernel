#include "include/paging.h"
#include "include/cr0.h"
#include "include/cpu.h"
#include "kernel/include/string.h"

static k_pde_t *k_page_table = NULL;

void k_paging_reserve_pages(k_uint32_t start, k_uint32_t range)
{
	k_uint32_t a, b;
	k_pde_t *pde;
	k_pte_t *pte;

	if (!k_page_table)
		return;

	a = start & ~0xfff;

	b = start + range;
	if (b & 0xfff)
		b &= ~0xfff;
	else
		b -= 0x1000;

	pde = k_page_table;

	while (1) {
		k_uint32_t table = (a >> 22) & 0x3ff;
		k_uint32_t page = (a >> 12) & 0x3ff;

		if ((pde[table] & K_PTE_P) == 0) {
			pde[table] = ((k_uint32_t)k_page_table + 0x1000 + table * 0x1000) |
				K_PDE_P | K_PDE_RW;

			k_memset((void *)(pde[table] & ~0xfff), 0, 0x1000);
		}

		pte = (k_pte_t *)(pde[table] & ~0xfff);
		pte[page] = (k_uint32_t)a | K_PTE_P | K_PTE_RW;

		if (a == b)
			break;

		a += 0x1000;
	}
}

void k_paging_table_set_start(k_uint32_t start)
{
	if (!k_page_table) {
		if (start & 0xfff)
			return;

		k_page_table = (void *)start;

		k_memset(k_page_table, 0, 0x1000);

		k_paging_reserve_pages(start, 0x1000 + 0x400 * 0x1000);
	}
}

void k_paging_init(void)
{
	k_uint32_t cr0;

	if (!k_page_table)
		return;

	asm volatile("mov %0, %%cr3" : : "r" (k_page_table));

	asm volatile("mov %%cr0, %0" : "=r" (cr0));

	cr0 |= K_CR0_PG;

	asm volatile("mov %0, %%cr0" : : "r" (cr0));
}

