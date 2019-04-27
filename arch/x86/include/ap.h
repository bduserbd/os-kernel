#ifndef K_AP_H
#define K_AP_H

#define K_AP_START_ADDRESS	0x8000
#define K_AP_START_INFO_OFFSET	0x180
#define K_AP_START_INFO		(K_AP_START_ADDRESS + K_AP_START_INFO_OFFSET)

#ifndef __ASSEMBLER__

#include "kernel/include/types.h"

struct k_ap_start_info {
	k_uint32_t page_table_physical;
	unsigned long entry;
	unsigned long sp;
} __attribute__((packed));

void k_ap_main(void);

#endif

#endif

