#include "kernel/include/time/clock.h"
#include "kernel/include/time/device.h"

static struct k_clock_device *k_clock_device = NULL;

k_uint64_t k_clock_now(void)
{
	return 0;
}

void k_clock_init(void)
{
	struct k_clock_device *device;

	device = k_clock_device_choose();
	if (!device)
		return;

	k_clock_device = device;
}

