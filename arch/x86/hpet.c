#include "include/hpet.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/time/time.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/divmod64.h"
#include "kernel/include/video/print.h"

static struct k_hpet_info {
	int timers;

	k_uint64_t frequency;

	unsigned long address;
} k_hpet;

static inline k_uint64_t k_hpet_get_reg(int reg)
{
	return *(volatile k_uint64_t *)(k_hpet.address + reg);
}

void k_hpet_init(void)
{
	k_uint64_t caps;
	k_uint64_t frequency;

	if (!k_acpi.found)
		return;

	k_memory_zone_dma_add(k_acpi.hpet_address >> 12, 1);
	k_hpet.address = k_p2v_l(k_acpi.hpet_address);

	caps = k_hpet_get_reg(K_HPET_CAPABILITIES);

	k_hpet.timers = K_HPET_NUM_TIM_CAP(caps);

	k_divmod64(K_FEMTOSECONDS_PER_SECOND, K_HPET_COUNTER_CLK_PERIOD(caps),
			&frequency, NULL);

	k_hpet.frequency = frequency;
}

