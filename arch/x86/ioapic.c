#include "include/ioapic.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

static struct k_ioapic_info {
	unsigned long address;
} k_ioapic;

static inline k_uint32_t k_ioapic_get_reg(int reg)
{
	*(volatile k_uint32_t *)(k_ioapic.address + K_IOAPIC_REGSEL) = reg;

	return *(volatile k_uint32_t *)(k_ioapic.address + K_IOAPIC_WIN);
}

static inline void k_ioapic_set_reg(int reg, k_uint32_t data)
{
	*(volatile k_uint32_t *)(k_ioapic.address + K_IOAPIC_REGSEL) = reg;
	*(volatile k_uint32_t *)(k_ioapic.address + K_IOAPIC_WIN) = data;
}

static inline k_uint64_t k_ioapic_get_redirect_reg(int reg)
{
	k_uint32_t a, b;

	a = k_ioapic_get_reg(K_IOAPIC_REDTBL + 2 * reg);
	b = k_ioapic_get_reg(K_IOAPIC_REDTBL + 2 * reg + 1);

	return (k_uint64_t)a << 32 | b;
}

void k_ioapic_init(void)
{
	if (!k_acpi.found)
		return;

	if (!(k_ioapic.address = k_p2v_l(k_acpi.ioapic_address))) {
		k_memory_zone_dma_add(k_acpi.ioapic_address >> 12, 1);
		k_ioapic.address = k_p2v_l(k_acpi.ioapic_address);
	}

	k_printf("I/O APIC: %x\n", k_ioapic.address);

	for (int i = 0; i < 24; i++)
		k_printf("%llx ", k_ioapic_get_redirect_reg(i));
}

