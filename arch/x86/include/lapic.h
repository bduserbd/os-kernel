#ifndef K_LAPIC_H
#define K_LAPIC_H

#include "include/error.h"
#include "include/types.h"

struct k_lapic_registers {
	__u8	reserved0[0x20];
} __attribute__((packed));

void k_lapic_init(void);

#endif

