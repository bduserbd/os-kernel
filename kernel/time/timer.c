#include "kernel/include/time/timer.h"
#include "kernel/include/time/device.h"
#include "kernel/include/time/clock.h"
#include "kernel/include/irq/irq.h"
#include "kernel/include/data-structures/heap/binary-heap.h"

static struct k_timer_device *k_timer_device = NULL;
static struct k_binary_heap *k_timers = NULL;

static k_error_t k_timer_device_irq_handler(unsigned int irq, void *data)
{
	return K_ERROR_NONE;
}

static int k_timers_compare(void *ptr1, void *ptr2)
{
	struct k_timer *timer1, *timer2;

	if (!ptr1 && ptr2)
		return -1;
	else if (ptr1 && !ptr2)
		return 1;
	else if (!ptr1 && !ptr2)
		return 0;
	else {
		timer1 = ptr1;
		timer2 = ptr2;

		if (timer1->expires < timer2->expires)
			return 1;
		else if (timer1->expires > timer2->expires)
			return -1;
		else
			return 0;
	}
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

	error = device->set_periodic_mode(device);
	if (error)
		return;

	k_timers = k_binary_heap_init(K_BINARY_HEAP_MIN, 255, k_timers_compare);
	if (!k_timers)
		return;

	k_timer_device = device;
}

void k_sleep(unsigned int milliseconds)
{
	struct k_timer *timer;
	struct k_timer timer1, timer2, timer3, timer4, timer5, timer6, timer7, timer8;

	timer1.expires = 54534;
	timer2.expires = 6905;
	timer3.expires = 790456;
	timer4.expires = 24;
	timer5.expires = 6e+9;
	timer6.expires = 7812;
	timer7.expires = 53;
	timer8.expires = 9799;

	k_binary_heap_insert(k_timers, &timer1);
	k_binary_heap_insert(k_timers, &timer2);
	k_binary_heap_insert(k_timers, &timer3);
	k_binary_heap_insert(k_timers, &timer4);
	k_binary_heap_insert(k_timers, &timer5);
	k_binary_heap_insert(k_timers, &timer6);
	k_binary_heap_insert(k_timers, &timer7);
	k_binary_heap_insert(k_timers, &timer8);

	while ((timer = k_binary_heap_fetch_root(k_timers)) != NULL)
		k_printf("%llu ", timer->expires);
}

