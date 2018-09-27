#include "include/lapic.h"
#include "include/cpu.h"
#include "include/ap.h"
#include "include/io.h"
#include "include/msr.h"
#include "kernel/include/video/print.h"
#include "kernel/include/acpi/acpi.h"

static struct k_lapic_info {
	int version;
	int max_lvt;

	k_uint32_t address;
} k_lapic;

static k_uint32_t k_lapic_read(int reg)
{
	return *(volatile k_uint32_t *)(k_lapic.address + reg);
}

static void k_lapic_write(int reg, k_uint32_t value)
{
	*(volatile k_uint32_t *)(k_lapic.address + reg) = value;
}

k_uint8_t k_lapic_id(void)
{
	return K_LAPIC_ID_VALUE(k_lapic_read(K_LAPIC_ID));
}

#ifdef K_CONFIG_SMP

void k_lapic_icr_init(k_uint8_t lapic_id)
{
	k_lapic_write(K_LAPIC_ICR2, lapic_id << 24);
	k_lapic_write(K_LAPIC_ICR, K_LAPIC_ICR_LEVEL_ASSERT |
			K_LAPIC_ICR_DELIVERY_MODE_INIT);

	while (k_lapic_read(K_LAPIC_ICR) & K_LAPIC_ICR_STATUS_PENDING) ;

	// TODO ..
	for (int i = 0; i < 100000000; i++) asm volatile("nop");
}

void k_lapic_icr_start_up(k_uint8_t lapic_id, k_uint8_t page)
{
	k_lapic_write(K_LAPIC_ICR2, lapic_id << 24);
	k_lapic_write(K_LAPIC_ICR, K_LAPIC_ICR_LEVEL_ASSERT |
			K_LAPIC_ICR_DELIVERY_MODE_START_UP | page);

	while (k_lapic_read(K_LAPIC_ICR) & K_LAPIC_ICR_STATUS_PENDING) ;

	// TODO ..
	for (int i = 0; i < 100000000; i++) asm volatile("nop");
}

#endif

void k_lapic_init(void)
{
	k_uint32_t svr, version;

	if (!k_acpi.found)
		return;

	/* Basic info. */
	k_lapic.address = k_acpi.lapic_address;
	k_puts("Local APIC: ");
	k_puthex(k_lapic.address);

	version = k_lapic_read(K_LAPIC_VERSION_REGISTER);
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

	/* Spurious vector. */
	svr = k_lapic_read(K_LAPIC_SVR);

	svr |= K_LAPIC_SVR_ENABLE;

	svr &= ~K_LAPIC_SVR_SPURIOUS_VECTOR_MASK;
	svr |= K_LAPIC_SVR_SPURIOUS_VECTOR;

	k_lapic_write(K_LAPIC_SVR, svr);
}

