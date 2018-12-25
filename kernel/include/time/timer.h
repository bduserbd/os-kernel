#ifndef K_TIMER_H
#define K_TIMER_H

#include "kernel/include/time/time.h"

struct k_timer {
	k_tick_t expires;

	struct k_timer *next;
};

void k_timer_init(void);

#endif

