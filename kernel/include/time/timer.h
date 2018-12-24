#ifndef K_TIMER_H
#define K_TIMER_H

#include "kernel/include/types.h"

struct k_timer {
	k_uint64_t expires;

	struct k_timer *next;
};

void k_timer_init(void);

#endif

