#ifndef K_INIT_X86_H
#define K_INIT_X86_H

#include "kernel/include/fb/shell.h"

#ifdef K_CONFIG_BIOS
#include "mp.h"
#endif

#ifdef K_CONFIG_SMP
#include "smp.h"
#endif

void k_x86_init(void *, void *, k_uint32_t, k_uint32_t);
k_error_t k_reserve_reserved_pages(void);

#endif

