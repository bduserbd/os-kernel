#ifndef K_PRINT_H
#define K_PRINT_H

#include <stdarg.h>
#include "kernel/include/types.h"

void k_print_set_output_callback(void (*)(const char *));

void k_puts(const char *);
void k_putchar(char);

void k_puthex(k_uint32_t, int);
void k_puthex64(k_uint64_t, int);

void k_putn(k_uint32_t, int);
void k_putn64(k_uint64_t, int);

int k_vprintf(const char *, va_list);
int k_printf(const char *, ...);

#endif

