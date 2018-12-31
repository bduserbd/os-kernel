#ifndef K_MM_H
#define K_MM_H

#include "kernel/include/types.h"
#include "kernel/include/mm/slab.h"

#define K_PAGE_NUMBER(address)	((address) >> 12)

void k_slab_init(void);

void *k_malloc(k_size_t);
void *k_zalloc(k_size_t);

void k_free(const void *);

void *k_buddy_alloc(k_size_t);
void *k_buddy_alloc_execute(k_size_t);
void k_buddy_free(const void *);

struct k_cache *k_cache_create(const char *, unsigned int, unsigned int, unsigned int);
void *k_cache_alloc(struct k_cache *);

void *k_p2v(const void *);
unsigned long k_p2v_l(unsigned long);

void *k_v2p(const void *);
unsigned long k_v2p_l(unsigned long);

void k_memory_zone_dma_add(unsigned long, unsigned long);

#endif

