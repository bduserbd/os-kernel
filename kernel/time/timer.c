#include "kernel/include/time/timer.h"
#include "kernel/include/time/device.h"
#include "kernel/include/time/clock.h"
#include "kernel/include/irq/irq.h"

static struct k_timer_device *k_timer_device = NULL;

static k_error_t k_timer_device_irq_handler(unsigned int irq, void *data)
{
	return K_ERROR_NONE;
}

void k_timer_init(void)
{
	k_error_t error;
	struct k_timer_device *device;

	device = k_timer_device_choose();
	if (!device)
		return;

	error = k_irq_request(device->irq, k_timer_device_irq_handler, 0, device);
	if (error)
		return;

	k_timer_device = device;
}

static struct k_timer *k_timers = NULL;

static void k_timer_register(struct k_timer *timer)
{
	struct k_timer **a, *b;

	if (!k_timers) {
		k_timers = timer;
		return;
	}

	for (a = &k_timers, b = k_timers; b; a = &b->next, b = b->next) {
		if (b == timer)
			return;

		if (timer->expires < b->expires)
			break;
	}

	timer->next = b;
	*a = timer;
}

void k_sleep(unsigned int milliseconds)
{
	static struct k_timer timer;

	timer.expires = k_clock_now() + milliseconds *
			(K_NANOSECONDS_PER_SECOND / K_MILLISECONDS_PER_SECOND);

	timer.next = NULL;

	k_timer_register(&timer);
}

