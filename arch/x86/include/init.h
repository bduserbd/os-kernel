#ifndef K_INIT_X86_H
#define K_INIT_X86_H

#include "kernel/include/fb/shell.h"

#ifdef K_CONFIG_SMP
#include "smp.h"
#endif

void k_x86_init(k_uint32_t, struct k_fb_info *, void *,
		k_uint32_t, k_uint32_t);

#endif

