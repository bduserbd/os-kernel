#include "include/time.h"

static struct k_timer_device *k_timer_device_scan(int type)
{
	struct k_timer_device *device;

	for (device = k_timer_devices; device; device = device->next)
		if (device->type == type)
			return device;

	return NULL;
}

struct k_timer_device *k_timer_device_choose(void)
{
	struct k_timer_device *device;

	(void)((device = k_timer_device_scan(K_TIMER_DEVICE_LAPIC)) ||
			(device = k_timer_device_scan(K_TIMER_DEVICE_HPET)) ||
			(device = k_timer_device_scan(K_TIMER_DEVICE_PIT)));

	return device;
}

