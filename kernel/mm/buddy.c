#include "include/mm/buddy.h"

void k_puts(const char *);
void k_puthex(k_uint32_t);

void __attribute__((weak)) k_paging_reserve_pages(k_uint32_t start, k_uint32_t range)
{

}

void k_buddy_init(k_uint32_t heap)
{
	k_puthex(K_BUDDY_SYSTEM_LIST_SIZE);
	k_puthex(heap);
}

