#ifndef K_SHELL_H
#define K_SHELL_H

#include "kernel/include/fb/fb.h"
#include "kernel/include/fb/font.h"

k_error_t k_shell_init(void);
void k_shell_puts(const char *);

#endif

