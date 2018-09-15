#include "include/lapic.h"
#include "include/cpu.h"
#include "include/msr.h"
#include "kernel/include/acpi/acpi.h"

void puts(const char *);
void puthex(k_uint32_t);

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

void k_lapic_init(void)
{
	k_uint32_t svr, version;

	if (!k_acpi.found)
		return;

	k_lapic.address = k_acpi.lapic_address;

	version = k_lapic_read(K_LAPIC_VERSION_REGISTER);
	k_lapic.version = K_LAPIC_VERSION_REGISTER_VERSION(version);
	k_lapic.max_lvt = 1 + K_LAPIC_VERSION_REGISTER_MAX_LVT(version);

	puthex(k_lapic.max_lvt);

	if (k_boot_cpu.family >= 6) {
		k_uint32_t low, high;

		k_rdmsr(K_MSR_IA32_APIC_BASE, &low, &high);

		if ((low & K_MSR_IA32_APIC_BASE_ENABLE) == 0) {
			low |= K_MSR_IA32_APIC_BASE_ENABLE;
			k_wrmsr(K_MSR_IA32_APIC_BASE, low, high);
		}
	}

	svr = k_lapic_read(K_LAPIC_SVR);
	puthex(svr);

	svr |= K_LAPIC_SVR_ENABLE;

	svr &= ~K_LAPIC_SVR_SPURIOUS_VECTOR_MASK;
	svr |= K_LAPIC_SVR_SPURIOUS_VECTOR;

	k_lapic_write(K_LAPIC_SVR, svr);
}

