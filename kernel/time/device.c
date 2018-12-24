#include "kernel/include/time/device.h"

struct k_clock_device *k_clock_devices = NULL;
struct k_timer_device *k_timer_devices = NULL;

struct k_clock_device *k_clock_device_choose(void)
{
	return k_clock_devices;
}

void k_clock_device_register(struct k_clock_device *device)
{
	device->next = k_clock_devices;
	k_clock_devices = device;
}

struct k_timer_device *k_timer_device_choose(void)
{
	return k_timer_devices;
}

void k_timer_device_register(struct k_timer_device *device)
{
	device->next = k_timer_devices;
	k_timer_devices = device;
}

