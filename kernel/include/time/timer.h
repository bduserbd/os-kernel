#ifndef K_TIMER_H
#define K_TIMER_H

#include "kernel/include/time/time.h"

struct k_timer {
	int expired;

	k_tick_t expire;

	struct k_timer *next;
};

static inline k_tick_t k_milliseconds_to_k_ticks(unsigned int milliseconds)
{
	if (K_HZ < K_MILLISECONDS_PER_SECOND)
		return K_MILLISECONDS_PER_SECOND * (milliseconds / K_HZ);
	else
		return K_HZ * (milliseconds / K_MILLISECONDS_PER_SECOND);
}

extern k_tick_t k_ticks;

void k_timer_init(void);

#endif

