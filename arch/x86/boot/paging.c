#include "include/paging.h"
#include "include/cr0.h"
#include "kernel/include/string.h"

extern __u8 __k_first_page_table[];

static void k_paging_get_pages(k_uint8_t *ptr, k_uint32_t *pages, k_uint32_t *dirs)
{
	*pages = K_ALIGN_UP((k_uint32_t)ptr, 0x1000) / 0x1000;
	*dirs = K_ALIGN_UP(*pages, 0x400) / 0x400;
}

void k_paging_init_boot(void)
{
	k_uint32_t i, j;
	k_uint32_t pages;
	k_uint32_t dirs;
	k_pde_t *pde;
	k_pte_t *pte;
	k_uint8_t *ptr;

	k_paging_get_pages(__k_first_page_table, &pages, &dirs);

	k_paging_get_pages(__k_first_page_table + K_PD_SIZE + dirs * K_PT_SIZE, &pages, &dirs);

	ptr = 0;
	pde = (k_pde_t *)__k_first_page_table;

	k_memset(__k_first_page_table, 0, K_PD_SIZE);

	for (i = 0; i < dirs; i++) {
		pde[i] = ((k_uint32_t)__k_first_page_table + (i + 1) * K_PD_SIZE) |
			K_PDE_P | K_PDE_RW;

		k_uint32_t to_map = K_MIN(pages, 0x400);
		pte = (k_pte_t *)(pde[i] & ~0xfff);

		k_memset(pte, 0, K_PT_SIZE);

		for (j = 0; j < to_map; j++) {
			pte[j] = (k_uint32_t)ptr | K_PTE_P | K_PTE_RW;
			ptr += 0x1000;
		}

		pages -= to_map;
	}

	asm volatile("mov %0, %%cr3" : : "r" (__k_first_page_table));

	k_uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= K_CR0_PG;

	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	//*(unsigned char *)((k_uint32_t)__k_first_page_table + K_PD_SIZE + dirs * K_PT_SIZE - 1)= 0xaa;
	//*(unsigned char *)((k_uint32_t)__k_first_page_table + K_PD_SIZE + dirs * K_PT_SIZE)= 0xaa;
}

