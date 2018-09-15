#ifndef K_LAPIC_H
#define K_LAPIC_H

#include "include/error.h"
#include "include/types.h"

#define K_LAPIC_VERSION_REGISTER	0x30
#define K_LAPIC_VERSION_REGISTER_VERSION(version) (version & 0xff)

#define K_LAPIC_SVR	0xf0
#define K_LAPIC_SVR_ENABLE			(1 << 8)
#define K_LAPIC_SVR_FOCUS_PROCESSOR_CHECKING	(1 << 9)
#define K_LAPIC_SVR_EOI_BROADCAST_SUPPRESSION	(1 << 12)
#define K_LAPIC_SVR_SPURIOUS_VECTOR_MASK	0xff
#define K_LAPIC_SVR_SPURIOUS_VECTOR		0xff

void k_lapic_init(void);

#endif

