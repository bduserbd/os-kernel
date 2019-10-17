#include "include/lapic.h"
#include "include/time.h"
#include "include/cpu.h"
#include "include/ap.h"
#include "include/msr.h"
#include "include/tsc.h"
#include "kernel/include/div64.h"
#include "kernel/include/io/io.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/time/device.h"

static struct k_lapic_info {
	int version;
	int max_lvt;

	k_uint64_t frequency;

	unsigned long address;
} k_lapic;

static k_uint32_t k_lapic_get_reg(int reg)
{
	return *(volatile k_uint32_t *)(k_lapic.address + reg);
}

static void k_lapic_set_reg(int reg, k_uint32_t value)
{
	*(volatile k_uint32_t *)(k_lapic.address + reg) = value;
}

k_uint8_t k_lapic_id(void)
{
	return K_LAPIC_ID_VALUE(k_lapic_get_reg(K_LAPIC_ID));
}

#ifdef K_CONFIG_SMP

void k_lapic_icr_init(k_uint8_t lapic_id)
{
	k_lapic_set_reg(K_LAPIC_ICR2, lapic_id << 24);
	k_lapic_set_reg(K_LAPIC_ICR, K_LAPIC_ICR_LEVEL_ASSERT |
			K_LAPIC_ICR_DELIVERY_MODE_INIT);

	while (k_lapic_get_reg(K_LAPIC_ICR) & K_LAPIC_ICR_STATUS_PENDING) ;
}

void k_lapic_icr_start_up(k_uint8_t lapic_id, k_uint8_t page)
{
	k_lapic_set_reg(K_LAPIC_ICR2, lapic_id << 24);
	k_lapic_set_reg(K_LAPIC_ICR, K_LAPIC_ICR_LEVEL_ASSERT |
			K_LAPIC_ICR_DELIVERY_MODE_START_UP | page);

	while (k_lapic_get_reg(K_LAPIC_ICR) & K_LAPIC_ICR_STATUS_PENDING) ;
}

#endif

void k_lapic_eoi(unsigned int irq)
{
	k_lapic_set_reg(K_LAPIC_EOI, 1 << irq);
}

static void k_lapic_basic_info_init(void)
{
	k_uint32_t version;

	version = k_lapic_get_reg(K_LAPIC_VERSION_REGISTER);
	k_lapic.version = K_LAPIC_VERSION_REGISTER_VERSION(version);
	k_lapic.max_lvt = 1 + K_LAPIC_VERSION_REGISTER_MAX_LVT(version);

	if (k_boot_cpu.family >= 6) {
		k_uint32_t low, high;

		k_rdmsr(K_MSR_IA32_APIC_BASE, &low, &high);

		if ((low & K_MSR_IA32_APIC_BASE_ENABLE) == 0) {
			low |= K_MSR_IA32_APIC_BASE_ENABLE;
			k_wrmsr(K_MSR_IA32_APIC_BASE, low, high);
		}
	}
}

void k_lapic_spurious_vector_init(void)
{
	k_uint32_t svr;

	svr = k_lapic_get_reg(K_LAPIC_SVR);

	svr |= K_LAPIC_SVR_ENABLE;

	svr &= ~K_LAPIC_SVR_SPURIOUS_VECTOR_MASK;
	svr |= K_LAPIC_SVR_SPURIOUS_VECTOR;

	k_lapic_set_reg(K_LAPIC_SVR, svr);
}

void k_my_lapic(void)
{
	k_uint32_t timer;
	k_uint64_t ticks;

	timer = K_LAPIC_LVT_TIMER_MODE_PERIODIC | k_irq_to_int(0);
	k_lapic_set_reg(K_LAPIC_LVT_TIMER, timer);

	k_lapic_set_reg(K_LAPIC_DIV_CONFIG, K_LAPIC_DIV_BY_16);

	k_div64(k_lapic.frequency, K_HZ, &ticks, NULL);

	k_lapic_set_reg(K_LAPIC_TIMER_ICR, (k_uint32_t)ticks);
}

static void k_lapic_timer_set_mode(struct k_timer_device *device, k_uint32_t mode)
{
	k_uint32_t timer;
	k_uint64_t ticks;

	timer = mode | k_irq_to_int(device->irq);
	k_lapic_set_reg(K_LAPIC_LVT_TIMER, timer);

	k_lapic_set_reg(K_LAPIC_DIV_CONFIG, K_LAPIC_DIV_BY_16);

	k_div64(k_lapic.frequency, K_HZ, &ticks, NULL);

	k_lapic_set_reg(K_LAPIC_TIMER_ICR, (k_uint32_t)ticks);
}

static k_error_t k_lapic_timer_set_periodic_mode(struct k_timer_device *device)
{
	k_lapic_timer_set_mode(device, K_LAPIC_LVT_TIMER_MODE_PERIODIC);

	return K_ERROR_NONE;
}

static k_error_t k_lapic_timer_set_oneshot_mode(struct k_timer_device *device)
{
	k_lapic_timer_set_mode(device, K_LAPIC_LVT_TIMER_MODE_ONESHOT);

	return K_ERROR_NONE;
}

static struct k_timer_device k_lapic_timer = {
	.type = K_TIMER_DEVICE_LAPIC,
	.flags = K_TIMER_DEVICE_FLAGS_PERIODIC | K_TIMER_DEVICE_FLAGS_ONESHOT,
	.irq = 0,
	.set_periodic_mode = k_lapic_timer_set_periodic_mode,
	.set_oneshot_mode = k_lapic_timer_set_oneshot_mode,
};

static k_uint32_t k_lapic_timer_count_ticks(void)
{
	k_uint32_t timer;

	timer = k_lapic_get_reg(K_LAPIC_LVT_TIMER);
	timer &= ~K_LAPIC_LVT_MASK;
	k_lapic_set_reg(K_LAPIC_LVT_TIMER, timer);

	k_lapic_set_reg(K_LAPIC_TIMER_ICR, K_LAPIC_TIMER_ICR_VALUE);

	k_tsc_sleep(1);

	timer |= K_LAPIC_LVT_MASK;
	k_lapic_set_reg(K_LAPIC_LVT_TIMER, timer);

	return K_LAPIC_TIMER_ICR_VALUE - k_lapic_get_reg(K_LAPIC_TIMER_CCR);
}

void k_lapic_timer_init(void)
{
	int i;
	k_uint64_t sum;

	k_lapic_set_reg(K_LAPIC_DIV_CONFIG, K_LAPIC_DIV_BY_16);

	k_lapic_timer_count_ticks();

	sum = 0;

#define LOG2_TIMES	3
	for (i = 0; i < (1 << LOG2_TIMES); i++) {
		k_uint32_t ccr = k_lapic_timer_count_ticks();
		//k_printf("%u ", ccr);
		sum += ccr;
	}

	k_lapic.frequency = (sum >> LOG2_TIMES) * K_MILLISECONDS_PER_SECOND;
	k_lapic_timer.frequency = k_lapic.frequency;

	k_timer_device_register(&k_lapic_timer);
}

k_error_t k_lapic_init(void)
{
	if (!k_acpi.found)
		return K_ERROR_NOT_FOUND;

	k_lapic.address = k_acpi.lapic_address;
	k_printf("Local APIC: %lx\n", k_lapic.address);

	k_lapic_basic_info_init();
	k_lapic_spurious_vector_init();
	k_lapic_timer_init();

	return K_ERROR_NONE;
}

