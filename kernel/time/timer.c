#include "kernel/include/time/timer.h"
#include "kernel/include/time/device.h"
#include "kernel/include/time/clock.h"
#include "kernel/include/irq/irq.h"
#include "kernel/include/data-structures/heap/binary-heap.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/modules/export-symbol.h"
#include "kernel/include/video/print.h"

k_tick_t k_ticks = 0;

static struct k_timer_device *k_timer_device = NULL;
static struct k_binary_heap *k_timers = NULL;

static void k_timer_check_expired(void)
{
	struct k_timer *timer;

	timer = k_binary_heap_get_root(k_timers);
	if (!timer)
		return;

	if (timer->expire <= k_ticks) {
		timer->expired = true;
		k_binary_heap_fetch_root(k_timers);
	}
}

static k_error_t k_timer_device_irq_handler(unsigned int irq, void *data)
{
	k_ticks++;

#if 0
	static int i = 0;

	i++;
	if ((i % (1 * 1000)) == 0)
		k_printf("%u", irq);
#endif

	k_timer_check_expired();

	return K_ERROR_NONE;
}

static int k_timers_compare(void *ptr1, void *ptr2)
{
	struct k_timer *timer1, *timer2;

	timer1 = ptr1;
	timer2 = ptr2;

	if (timer1->expire < timer2->expire)
		return 1;
	else if (timer1->expire > timer2->expire)
		return -1;
	else
		return 0;
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

	k_timers = k_binary_heap_init(K_BINARY_HEAP_MIN, 0x3, k_timers_compare);
	if (!k_timers)
		return;

	k_timer_device = device;
}

void k_sleep(unsigned int milliseconds)
{
	k_error_t error;
	struct k_timer timer;

	//timer = k_malloc(sizeof(struct k_timer));
	//if (!timer)
	//	return;

	timer.expired = false;
	timer.expire = k_ticks + k_milliseconds_to_k_ticks(milliseconds);

	error = k_binary_heap_insert(k_timers, &timer);
	if (error)
		return;

	while (!timer.expired) ;
}
K_EXPORT_FUNC(k_sleep);

