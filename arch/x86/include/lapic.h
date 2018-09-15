#ifndef K_LAPIC_H
#define K_LAPIC_H

#include "include/error.h"
#include "include/types.h"

/* Local APIC Version Register. */
#define K_LAPIC_VERSION_REGISTER	0x30

#define K_LAPIC_VERSION_REGISTER_VERSION(version) (version & 0xff)
#define K_LAPIC_VERSION_REGISTER_MAX_LVT(version) ((version >> 16) & 0xff)

/* Local APIC Spurious Interrupt Vector Register. */
#define K_LAPIC_SVR	0xf0

#define K_LAPIC_SVR_ENABLE			(1 << 8)
#define K_LAPIC_SVR_FOCUS_PROCESSOR_CHECKING	(1 << 9)
#define K_LAPIC_SVR_EOI_BROADCAST_SUPPRESSION	(1 << 12)
#define K_LAPIC_SVR_SPURIOUS_VECTOR_MASK	0xff
#define K_LAPIC_SVR_SPURIOUS_VECTOR		0xff

/* Local APIC Error Status Register. */
#define K_LAPIC_ESR	0x280

#define K_LAPIC_ESR_SEND_CHECKSUM_ERROR		(1 << 0)
#define K_LAPIC_ESR_RECEIVE_CHECKSUM_ERROR	(1 << 1)
#define K_LAPIC_ESR_SEND_ACCEPT_ERROR		(1 << 2)
#define K_LAPIC_ESR_RECEIVE_ACCEPT_ERROR	(1 << 3)

/* Local APIC LVT CMCI Register. */
#define K_LAPIC_LVT_CMCI	0x2f0

/* Local APIC Interrupt Command Register. */
#define K_LAPIC_ICR	0x300

#define K_LAPIC_ICR_DELIVERY_MODE_FIXED			(0 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_LOWEST_PRIORITY	(1 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_SMI			(2 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_NMI			(4 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_INIT			(5 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_START_UP		(6 << 8)

#define K_LAPIC_ICR2	0x310

/* Local APIC LVT Timer Register. */
#define K_LAPIC_LVT_TIMER	0x320

/* Local APIC LVT LINT0 Register. */
#define K_LAPIC_LVT_LINT0	0x350

/* Local APIC LVT LINT1 Register. */
#define K_LAPIC_LVT_LINT1	0x360

/* Local APIC Divide Configuration Register. */
#define K_LAPIC_DIV_CONFIG	0x3e0

#define K_LAPIC_DIV_BY_2	0x0
#define K_LAPIC_DIV_BY_4	0x1
#define K_LAPIC_DIV_BY_8	0x2
#define K_LAPIC_DIV_BY_16	0x3
#define K_LAPIC_DIV_BY_32	0x8
#define K_LAPIC_DIV_BY_64	0x9
#define K_LAPIC_DIV_BY_128	0xa
#define K_LAPIC_DIV_BY_1	0xb

void k_lapic_init(void);

#endif

