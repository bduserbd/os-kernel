#ifndef K_LAPIC_H
#define K_LAPIC_H

#include "kernel/include/error.h"
#include "kernel/include/types.h"

/* Local APIC ID. */
#define K_LAPIC_ID	0x20
#define K_LAPIC_ID_VALUE(id)	((id >> 24) & 0xff)

/* Local APIC Version Register. */
#define K_LAPIC_VERSION_REGISTER	0x30

#define K_LAPIC_VERSION_REGISTER_VERSION(version) (version & 0xff)
#define K_LAPIC_VERSION_REGISTER_MAX_LVT(version) ((version >> 16) & 0xff)

/* Local APIC Task Priority Register. */
#define K_LAPIC_TPR	0x80

/* Local APIC End of Interrupt Register. */
#define K_LAPIC_EOI	0xb0

/* Local APIC Logical Destination Register.  */
#define K_LAPIC_LDR	0xd0

/* Local APIC Destination Format Register.*/
#define K_LAPIC_DFR	0xe0

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

/* Local APIC Interrupt Command Register. */
#define K_LAPIC_ICR	0x300

#define K_LAPIC_ICR_DELIVERY_MODE_FIXED			(0 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_LOWEST_PRIORITY	(1 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_SMI			(2 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_NMI			(4 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_INIT			(5 << 8)
#define K_LAPIC_ICR_DELIVERY_MODE_START_UP		(6 << 8)

#define K_LAPIC_ICR_DEST_MODE_PHYSICAL	(0 << 11)
#define K_LAPIC_ICR_DEST_MODE_LOGICAL	(1 << 11)

#define K_LAPIC_ICR_STATUS_IDLE		(0 << 12)
#define K_LAPIC_ICR_STATUS_PENDING	(1 << 12)

#define K_LAPIC_ICR_LEVEL_DEASSERT	(0 << 14)
#define K_LAPIC_ICR_LEVEL_ASSERT	(1 << 14)

#define K_LAPIC_ICR_TRIGGER_EDGE	(0 << 15)
#define K_LAPIC_ICR_TRIGGER_LEVEL	(1 << 15)

#define K_LAPIC_ICR_DEST_NO_SHORTHAND		(0 << 18)
#define K_LAPIC_ICR_DEST_SELF			(1 << 18)
#define K_LAPIC_ICR_DEST_ALL_INCLUDING_SELF	(2 << 18)
#define K_LAPIC_ICR_DEST_ALL_EXCLUDING_SELF	(3 << 18)

#define K_LAPIC_ICR2	0x310

/* Local APIC LVT. */
#define K_LAPIC_LVT_MASK	(1 << 16)

/* Local APIC LVT CMCI Register. */
#define K_LAPIC_LVT_CMCI	0x2f0

/* Local APIC LVT Timer Register. */
#define K_LAPIC_LVT_TIMER	0x320

#define K_LAPIC_LVT_TIMER_MODE_ONESHOT		(0 << 17)
#define K_LAPIC_LVT_TIMER_MODE_PERIODIC		(1 << 17)
#define K_LAPIC_LVT_TIMER_MODE_TSC_DEADLINE	(2 << 17)

/* Local APIC LVT LINT0 Register. */
#define K_LAPIC_LVT_LINT0	0x350

/* Local APIC LVT LINT1 Register. */
#define K_LAPIC_LVT_LINT1	0x360

/* Local APIC Initial Counter Register. */
#define K_LAPIC_TIMER_ICR	0x380

#define K_LAPIC_TIMER_ICR_VALUE	0xffffffff

/* Local APIC Current Counter Register. */
#define K_LAPIC_TIMER_CCR	0x390

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

k_uint8_t k_lapic_id(void);
void k_lapic_icr_init(k_uint8_t);
void k_lapic_icr_start_up(k_uint8_t, k_uint8_t);

void k_lapic_eoi(unsigned int);

#endif

