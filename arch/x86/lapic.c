#include "include/lapic.h"
#include "include/msr.h"

void puts(const char *);
void puthex(k_uint32_t);

void k_lapic_init(void)
{
	k_uint32_t low, high;

	k_rdmsr(K_MSR_IA32_APIC_BASE, &low, &high);

	if ((low & K_MSR_IA32_APIC_BASE_ENABLE) == 0) {
		low |= K_MSR_IA32_APIC_BASE_ENABLE;
		k_wrmsr(K_MSR_IA32_APIC_BASE, low, high);
	}
}

