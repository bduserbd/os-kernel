#include "include/lapic.h"
#include "include/cpu.h"
#include "include/msr.h"
#include "kernel/include/acpi/acpi.h"

void puts(const char *);
void puthex(k_uint32_t);

static k_uint32_t k_lapic_address;

static k_uint32_t k_lapic_read(int reg)
{
	return *(volatile k_uint32_t *)(k_lapic_address + reg);
}

static void k_lapic_write(int reg, k_uint32_t value)
{
	*(volatile k_uint32_t *)(k_lapic_address + reg) = value;
}

void k_lapic_init(void)
{
	k_uint32_t svr;

	if (!k_acpi.found)
		return;

	k_lapic_address = k_acpi.lapic_address;

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

