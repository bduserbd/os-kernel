#include "include/hpet.h"
#include "include/time.h"
#include "include/paging.h"
#include "kernel/include/bits.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/time/device.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/div64.h"
#include "kernel/include/video/print.h"

static struct k_hpet_info {
	int timers;

	k_uint32_t period;
	k_uint64_t frequency;

	unsigned long address;
} k_hpet;

static struct k_clock_device k_hpet_clock;
static struct k_timer_device k_hpet_timer;

static inline k_uint64_t k_hpet_get_reg(int reg)
{
	return *(volatile k_uint64_t *)(k_hpet.address + reg);
}

static inline void k_hpet_set_reg(int reg, k_uint64_t data)
{
	*(volatile k_uint64_t *)(k_hpet.address + reg) = data;
}

static void k_hpet_stop_main_counter(void)
{
	k_uint64_t config;

	config = k_hpet_get_reg(K_HPET_CONFIGURATION);
	config &= ~K_HPET_ENABLE_CNF;
	k_hpet_set_reg(K_HPET_CONFIGURATION, config);
}

static void k_hpet_start_main_counter(void)
{
	k_uint64_t config;

	config = k_hpet_get_reg(K_HPET_CONFIGURATION);
	config |= K_HPET_ENABLE_CNF;
	k_hpet_set_reg(K_HPET_CONFIGURATION, config);
}

static void k_hpet_set_periodic_mode_index(int timer)
{
	k_uint64_t config;
	k_uint64_t counter, ticks;

	config = k_hpet_get_reg(K_HPET_TIMER_CONFIGURATION(timer));
	if (!(config & K_HPET_TIMER_PER_INT_CAP))
		return;

	k_hpet_stop_main_counter();

	counter = k_hpet_get_reg(K_HPET_MAIN_COUNTER);
	k_div64(k_hpet.frequency, K_HZ, &ticks, NULL);

	config |= K_HPET_TIMER_INT_ENB_CNF | K_HPET_TIMER_TYPE_CNF |
			K_HPET_TIMER_VAL_SET_CNF;

	k_hpet_set_reg(K_HPET_TIMER_CONFIGURATION(timer), config);
	k_hpet_set_reg(K_HPET_TIMER_COMPARATOR(timer), counter + ticks);
	k_hpet_set_reg(K_HPET_TIMER_COMPARATOR(timer), ticks);

	k_hpet_start_main_counter();
}

static k_error_t k_hpet_set_periodic_mode(struct k_timer_device *timer)
{
	k_hpet_set_periodic_mode_index(0);

	return K_ERROR_NONE;
}

static void k_hpet_set_oneshot_mode_index(int timer)
{
	k_uint64_t config;

	config = k_hpet_get_reg(K_HPET_TIMER_CONFIGURATION(timer));

	if (config & K_HPET_TIMER_PER_INT_CAP) {
		config &= ~K_HPET_TIMER_TYPE_CNF;
		k_hpet_set_reg(K_HPET_TIMER_CONFIGURATION(timer), config);
	}
}

static k_error_t k_hpet_set_oneshot_mode(struct k_timer_device *timer)
{
	k_hpet_set_oneshot_mode_index(0);

	return K_ERROR_NONE;
}

static void k_hpet_timers_init(void)
{
	int i;
	unsigned int irq;
	k_uint64_t config;

	for (i = 0; i < 1 /* k_hpet.timers */ ; i++) {
		config = k_hpet_get_reg(K_HPET_TIMER_CONFIGURATION(i));
		config &= ~K_HPET_TIMER_INT_ROUTE_CNF_MASK;

		irq = k_bit_scan_forward(K_HPET_TIMER_INT_ROUTE_CAP(config));

		k_hpet_timer.irq = irq;

		config |= K_HPET_TIMER_INT_ROUTE_CNF(irq);
		k_hpet_set_reg(K_HPET_TIMER_CONFIGURATION(i), config);
	}
}

static void k_hpet_info_init(void)
{
	k_uint64_t caps;
	k_uint64_t frequency;

	caps = k_hpet_get_reg(K_HPET_CAPABILITIES);

	k_hpet.period = K_HPET_COUNTER_CLK_PERIOD(caps);

	k_div64(K_FEMTOSECONDS_PER_SECOND, k_hpet.period, &frequency, NULL);
	k_hpet.frequency = frequency;

	k_hpet_clock.frequency = k_hpet.frequency;
	k_hpet_timer.frequency = k_hpet.frequency;

	k_hpet.timers = K_HPET_NUM_TIM_CAP(caps);
	k_hpet_timers_init();
}

static k_tick_t k_hpet_get_tick(struct k_clock_device *clock)
{
	return k_hpet_get_reg(K_HPET_MAIN_COUNTER);
}

static struct k_clock_device k_hpet_clock = {
	.type = K_CLOCK_DEVICE_HPET,
	.get_tick = k_hpet_get_tick,
};

static struct k_timer_device k_hpet_timer = {
	.type = K_TIMER_DEVICE_HPET,
	.flags = K_TIMER_DEVICE_FLAGS_PERIODIC | K_TIMER_DEVICE_FLAGS_ONESHOT,
	.set_periodic_mode = k_hpet_set_periodic_mode,
	.set_oneshot_mode = k_hpet_set_oneshot_mode,
};

k_error_t k_hpet_init(void)
{
	if (!k_acpi.found)
		return K_ERROR_NOT_FOUND;

	k_memory_zone_dma_add(K_PAGE_NUMBER(k_acpi.hpet_address), 1);
	k_hpet.address = k_p2v_l(k_acpi.hpet_address);

	k_hpet_stop_main_counter();

	k_hpet_info_init();

	k_hpet_set_reg(K_HPET_MAIN_COUNTER, 0);
	k_hpet_start_main_counter();

	k_clock_device_register(&k_hpet_clock);
	k_timer_device_register(&k_hpet_timer);

	return K_ERROR_NONE;
}

