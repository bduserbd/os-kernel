#include "include/ioapic.h"
#include "include/lapic.h"
#include "include/idt.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

#ifdef K_CONFIG_BIOS
#include "include/mp.h"
#endif

static struct k_ioapic_info {
	int redirect_entries;
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
	int offset;
	k_uint32_t a, b;

	offset = reg << 1;

	a = k_ioapic_get_reg(K_IOAPIC_REDTBL + offset);
	b = k_ioapic_get_reg(K_IOAPIC_REDTBL + offset + 1);

	return (k_uint64_t)b << 32 | a;
}

static inline void k_ioapic_set_redirect_reg(int reg, k_uint64_t data)
{
	int offset;

	offset = reg << 1;

	k_ioapic_set_reg(K_IOAPIC_REDTBL + offset, (k_uint32_t)data);
	k_ioapic_set_reg(K_IOAPIC_REDTBL + offset + 1, (k_uint32_t)(data >> 32));
}

static void k_ioapic_ack(struct k_irq_chip *chip, unsigned int irq)
{
	k_lapic_eoi(irq);
}

static void k_ioapic_mask(struct k_irq_chip *chip, unsigned int irq)
{
	k_uint64_t data;

	data = k_ioapic_get_redirect_reg(irq);
	data |= K_IOAPIC_REDTBL_INTERRUPT_MASK;
	k_ioapic_set_redirect_reg(irq, data);
}

static void k_ioapic_unmask(struct k_irq_chip *chip, unsigned int irq)
{
	k_uint64_t data;

	data = k_ioapic_get_redirect_reg(irq);
	data &= ~K_IOAPIC_REDTBL_INTERRUPT_MASK;
	k_ioapic_set_redirect_reg(irq, data);
}

static void k_ioapic_reset(struct k_irq_chip *chip)
{

}

static k_error_t k_ioapic_start(struct k_irq_chip *chip)
{
	int i;

	for (i = 0; i < k_ioapic.redirect_entries; i++)
		k_ioapic_set_redirect_reg(i, K_IOAPIC_REDTBL_INTVEC(K_IRQ_MASTER_START + i) |
						K_IOAPIC_REDTBL_DELMOD_FIXED |
						K_IOAPIC_REDTBL_DESTMOD_PHYSICAL |
						(0ULL << 56));

	return K_ERROR_NONE;
}

static unsigned int k_ioapic_int_to_irq(struct k_irq_chip *chip, unsigned int interrupt)
{
	return interrupt - K_IRQ_MASTER_START;
}

static struct k_irq_chip k_ioapic_irq_chip = {
	.name = "I/O APIC",
	.start = k_ioapic_start,
	.reset = k_ioapic_reset,
	.ack = k_ioapic_ack,
	.mask = k_ioapic_mask,
	.unmask = k_ioapic_unmask,
	.int_to_irq = k_ioapic_int_to_irq,
	.data = NULL,
};

void k_ioapic_init(void)
{
	k_uint32_t version;

	if (!k_acpi.found)
		return;

#ifdef K_CONFIG_BIOS
	if (!k_mp.found)
		return;

	if (k_acpi.ioapic_address != k_mp.ioapic_address)
		return;
#endif

	if (!(k_ioapic.address = k_p2v_l(k_acpi.ioapic_address))) {
		k_memory_zone_dma_add(k_acpi.ioapic_address >> 12, 1);
		k_ioapic.address = k_p2v_l(k_acpi.ioapic_address);
	}

	version = k_ioapic_get_reg(K_IOAPIC_VER);
	k_ioapic.redirect_entries = K_IOAPIC_VER_MAX_REDIRECT_ENTRIES(version);

	k_ioapic_irq_chip.irqs = k_ioapic.redirect_entries;

	k_irq_register_chip(&k_ioapic_irq_chip);

	k_printf("I/O APIC: %lx\n", k_ioapic.address);
}

