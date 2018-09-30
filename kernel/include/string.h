#ifndef K_STRING_H
#define K_STRING_H

#include "kernel/include/types.h"

void *k_memset(void *, int, k_size_t);
void *k_memcpy(void *, const void *, k_size_t);
int k_memcmp(const void *, const void *, k_size_t);
int k_strcmp(const char *, const char *);
int k_strncmp(const char *, const char *, k_size_t);

#endif

