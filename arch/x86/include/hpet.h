#ifndef K_HPET_H
#define K_HPET_H

/* HPET registers. */
enum {
	K_HPET_CAPABILITIES	= 0x00,
	K_HPET_CONFIGURATION	= 0x10,
	K_HPET_INTERRUPT_STATUS	= 0x20,
	K_HPET_MAIN_COUNTER	= 0xf0,
};

/* HPET general capabilities and ID register. */
#define K_HPET_NUM_TIM_CAP(caps)	(((caps >> 8) & 0x1f) + 1)
#define K_HPET_COUNTER_CLK_PERIOD(caps)	((caps >> 32) & 0xffffffff)

/* HPET general configuration register. */
#define K_HPET_ENABLE_CNF	(1 << 0)
#define K_HPET_LEG_RT_CNF	(1 << 1)

/* HPET timer configuration and capability register. */
#define K_HPET_TIMER_CONFIGURATION(i)	(0x100 + i * 0x20)

#define K_HPET_TIMER_INT_TYPE_CNF_EDGE	(0 << 1)
#define K_HPET_TIMER_INT_TYPE_CNF_LEVEL	(1 << 1)

#define K_HPET_TIMER_INT_ENB_CNF	(1 << 2)

#define K_HPET_TIMER_TYPE_CNF		(1 << 3)

#define K_HPET_TIMER_PER_INT_CAP	(1 << 4)

#define K_HPET_TIMER_SIZE_CAP_32	(0 << 5)
#define K_HPET_TIMER_SIZE_CAP_64	(1 << 5)

/* HPET timer comparator register. */
#define K_HPET_TIMER_COMPARATOR(i)	(0x100 + i * 0x20 + 0x8)

/* HPET timer FSB interrupt route register. */
#define K_HPET_TIMER_INTERRUPT_ROUTE(i)	(0x100 + i * 0x20 + 0x10)

void k_hpet_init(void);

#endif

