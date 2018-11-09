#ifndef K_MM_H
#define K_MM_H

#include "kernel/include/types.h"

void k_slab_init(void);

void *k_malloc(k_size_t);
void k_free(const void *);

void *k_p2v(const void *);
unsigned long k_p2v_l(unsigned long);

void *k_v2p(const void *);
unsigned long k_v2p_l(unsigned long);

#endif

