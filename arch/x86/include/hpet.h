#ifndef K_HPET_H
#define K_HPET_H

/* HPET registers. */
enum {
	K_HPET_CAPABILITIES	= 0x00,
	K_HPET_CONFIGURATION	= 0x10,
	K_HPET_INTERRUPT_STATUS	= 0x20,
	K_HPET_MAIN_COUNTER	= 0x20,
};

/* HPET general capabilities and ID register. */
#define K_HPET_NUM_TIM_CAP(caps)	(((caps >> 8) & 0x1f) + 1)

/* HPET timer configuration and capability register. */
#define K_HPET_TIMER_CONFIGURATION(i)	(0x100 + i * 0x20)

/* HPET timer comparator register. */
#define K_HPET_TIMER_COMPARATOR(i)	(0x100 + i * 0x20 + 0x8)

/* HPET timer FSB interrupt route register. */
#define K_HPET_TIMER_INTERRUPT_ROUTE(i)	(0x100 + i * 0x20 + 0x10)

void k_hpet_init(void);

#endif

