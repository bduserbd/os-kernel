#ifndef K_TIME_DEVICE_H
#define K_TIME_DEVICE_H

#include "kernel/include/error.h"
#include "kernel/include/time/time.h"

enum {
	K_CLOCK_DEVICE_HPET,
	K_CLOCK_DEVICE_TSC,
};

struct k_clock_device {
	int type;

	k_uint64_t frequency;
	k_tick_t (*get_tick)(struct k_clock_device *);

	struct k_clock_device *next;
};

enum {
	K_TIMER_DEVICE_PIT,
	K_TIMER_DEVICE_HPET,
};

#define K_TIMER_DEVICE_FLAGS_PERIODIC	(1 << 0)
#define K_TIMER_DEVICE_FLAGS_ONESHOT	(1 << 1)

struct k_timer_device {
	int type;

	unsigned int flags;

	unsigned int irq;

	k_uint64_t frequency;

	k_error_t (*set_periodic_mode)(struct k_timer_device *);

	k_error_t (*set_oneshot_mode)(struct k_timer_device *);

	struct k_timer_device *next;
};

extern struct k_clock_device *k_clock_devices;
extern struct k_timer_device *k_timer_devices;

struct k_clock_device *k_clock_device_choose(void);
struct k_timer_device *k_timer_device_choose(void);

void k_clock_device_register(struct k_clock_device *);
void k_timer_device_register(struct k_timer_device *);

#endif

