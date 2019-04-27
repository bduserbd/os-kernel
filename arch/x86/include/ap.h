#ifndef K_AP_H
#define K_AP_H

#include "kernel/include/types.h"

#define K_AP_START_ADDRESS	0x8000

struct k_ap_start_info {
	unsigned long entry;
	unsigned long sp;
	int processor;
	unsigned long page_table_physical;
} __attribute__((packed));

void k_ap_main(k_uint32_t);

#endif

