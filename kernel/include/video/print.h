#ifndef K_PRINT_H
#define K_PRINT_H

#include "kernel/include/types.h"

void k_print_set_output_callback(void (*)(const char *));

void k_puts(const char *);
void k_puthex(k_uint32_t);
void k_puthex64(k_uint64_t);

#endif

