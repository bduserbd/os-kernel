#ifndef K_X86_TIME_H
#define K_X86_TIME_H

#include "kernel/include/time/device.h"

enum {
	K_CLOCK_DEVICE_HPET,
	K_CLOCK_DEVICE_TSC,
};

enum {
	K_TIMER_DEVICE_PIT,
	K_TIMER_DEVICE_HPET,
	K_TIMER_DEVICE_LAPIC
};

#endif

