#ifndef K_TIMER_H
#define K_TIMER_H

#include "kernel/include/time/time.h"

struct k_clock_device {
	const char *name;

	struct k_clock_device *next;
};

#define K_TIMER_DEVICE_FLAGS_PERIODIC	(1 << 0)
#define K_TIMER_DEVICE_FLAGS_ONESHOT	(1 << 1)

struct k_timer_device {
	const char *name;

	unsigned int flags;

	unsigned int irq;

	struct k_timer_device *next;
};

#endif

