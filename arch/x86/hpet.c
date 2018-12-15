#include "include/hpet.h"
#include "kernel/include/bits.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/time/device.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/divmod64.h"
#include "kernel/include/video/print.h"

static struct k_hpet_info {
	int timers;

	k_uint32_t period;
	k_uint64_t frequency;

	unsigned long address;
} k_hpet;

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
	ticks = 1000000;

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
	k_uint32_t irq_bits;
	k_uint64_t config;

	for (i = 0; i < 1 /* k_hpet.timers */ ; i++) {
		config = k_hpet_get_reg(K_HPET_TIMER_CONFIGURATION(i));
		config &= ~(K_HPET_TIMER_INT_ROUTE_CNF_MASK);

		irq_bits = K_HPET_TIMER_INT_ROUTE_CAP(config);

		config |= K_HPET_TIMER_INT_ROUTE_CNF(6 /* k_bit_scan_forward(irq_bits) */);
		k_hpet_set_reg(K_HPET_TIMER_CONFIGURATION(i), config);

		k_hpet_set_periodic_mode_index(i);
	}
}

static void k_hpet_info_init(void)
{
	k_uint64_t caps;
	k_uint64_t frequency;

	caps = k_hpet_get_reg(K_HPET_CAPABILITIES);

	k_hpet.period = K_HPET_COUNTER_CLK_PERIOD(caps);

	k_divmod64(K_FEMTOSECONDS_PER_SECOND, k_hpet.period, &frequency, NULL);

	k_hpet.frequency = frequency;

	k_hpet.timers = K_HPET_NUM_TIM_CAP(caps);
	k_hpet_timers_init();
}

#define HPET	"HPET - High Precision Event Timer"

#if 0
static struct k_clock_device k_clock_hpet = {
	.name = HPET,
};
#endif

static struct k_timer_device k_timer_hpet = {
	.name = HPET,
	.flags = K_TIMER_DEVICE_FLAGS_PERIODIC | K_TIMER_DEVICE_FLAGS_ONESHOT,
	.irq = 0,
	.set_periodic_mode = k_hpet_set_periodic_mode,
	.set_oneshot_mode = k_hpet_set_oneshot_mode,
};

void k_hpet_init(void)
{
	if (!k_acpi.found)
		return;

	k_memory_zone_dma_add(k_acpi.hpet_address >> 12, 1);
	k_hpet.address = k_p2v_l(k_acpi.hpet_address);

	k_hpet_stop_main_counter();

	k_hpet_info_init();

	k_hpet_set_reg(K_HPET_MAIN_COUNTER, 0);
	k_hpet_start_main_counter();

	k_printf("%llu\n", k_hpet_get_reg(K_HPET_MAIN_COUNTER));
	k_printf("%llu\n", k_hpet_get_reg(K_HPET_MAIN_COUNTER));
}

