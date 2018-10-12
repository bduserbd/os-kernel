#ifndef K_MM_H
#define K_MM_H

#include "kernel/include/types.h"

void k_slab_init(void);

void *k_malloc(k_size_t);
void k_free(const void *);

#endif

