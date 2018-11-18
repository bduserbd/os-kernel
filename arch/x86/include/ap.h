#ifndef K_AP_H
#define K_AP_H

#include "kernel/include/types.h"

struct k_ap_start_info {
	k_uint32_t entry;
	k_uint32_t esp;
	k_uint32_t processor;
} __attribute__((packed));

void k_ap_main(k_uint32_t);

#endif

