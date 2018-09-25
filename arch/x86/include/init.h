#ifndef K_INIT_X86_H
#define K_INIT_X86_H

#include "kernel/include/types.h"

#ifdef K_CONFIG_BIOS
void k_x86_init(k_uint32_t);
#elif K_CONFIG_UEFI

#include "kernel/include/fb/fb.h"

void k_x86_init(k_uint32_t, struct k_fb_info *);

#endif

#endif

