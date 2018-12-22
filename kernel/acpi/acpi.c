#include "include/acpi/acpi.h"
#include "include/string.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/video/print.h"

struct k_acpi_info k_acpi;

static k_error_t k_acpi_checksum(void *ptr, int length)
{
	int i;
	k_uint8_t sum;

	sum = 0;

	for (i = 0; i < length; i++)
		sum += *((k_uint8_t *)ptr + i);

	if (!sum)
		return K_ERROR_NONE;
	else
		return K_ERROR_BAD_CHECKSUM;
}

static void k_acpi_parse_lapic_nmi(struct k_acpi_lapic_nmi *lapic_nmi)
{

}

static void k_acpi_parse_interrupt_override(struct k_acpi_interrupt_override *interrupt)
{

}

static void k_acpi_parse_ioapic(struct k_acpi_ioapic *ioapic)
{
	k_acpi.ioapic_address = ioapic->ioapic_address;
}

static k_error_t k_acpi_parse_lapic(int index, struct k_acpi_lapic *lapic)
{
	if ((lapic->flags & K_ACPI_LAPIC_FLAGS_ENABLED) == 0)
		return K_ERROR_INVALID_DEVICE;

	k_acpi.ids[index].valid = true;
	k_acpi.ids[index].processor = lapic->acpi_processor_id;
	k_acpi.ids[index].lapic = lapic->apic_id;

	return K_ERROR_NONE;
}

static void k_acpi_parse_madt(struct k_acpi_madt *madt)
{
	int i;
	int lapic_count = 0;
	k_error_t error;
	k_uint8_t *type, length;

	error = k_acpi_checksum(madt, madt->sdt.length);
	if (error)
		return;

	for (i = 0; i < K_CONFIG_CPUS; i++)
		k_acpi.ids[i].valid = false;

	k_acpi.lapic_address = k_p2v_l(madt->lapic_address);

	for (type = &madt->entries[0], length = *(k_uint8_t *)(type + 1);
		type - (k_uint8_t *)madt < madt->sdt.length;
		type += length, length = *(k_uint8_t *)(type + 1)) {
		switch (*type) {
		case K_ACPI_MADT_LAPIC:
			if (lapic_count < K_CONFIG_CPUS) {
				error = k_acpi_parse_lapic(lapic_count, (void *)type);
				if (!error)
					lapic_count++;
			}

			break;

		case K_ACPI_MADT_IOAPIC:
			k_acpi_parse_ioapic((struct k_acpi_ioapic *)type);
			break;

		case K_ACPI_MADT_INTERRUPT_OVERRIDE:
			k_acpi_parse_interrupt_override((struct k_acpi_interrupt_override *)type);
			break;

		case K_ACPI_MADT_LAPIC_NMI:
			k_acpi_parse_lapic_nmi((struct k_acpi_lapic_nmi *)type);
			break;
		}
	}
}

static void k_acpi_parse_hpet(struct k_acpi_hpet *hpet)
{
	if (hpet->address.space_id != K_ACPI_ADDRESS_SPACE_ID_SYSTEM_MEMORY)
		return;

	k_acpi.hpet_address = hpet->address.address;
}

static void k_acpi_parse_xsdt(struct k_acpi_xsdt *xsdt)
{
	k_uint32_t i;
	k_error_t error;
	struct k_acpi_sdt *sdt;

	if (k_memcmp((void *)xsdt->sdt.signature, K_ACPI_XSDT_SIGNATURE, 4))
		return;

	error = k_acpi_checksum(xsdt, xsdt->sdt.length);
	if (error)
		return;

	for (i = 0; i < (xsdt->sdt.length - K_OFFSETOF(struct k_acpi_xsdt, entries)) /
		sizeof(k_uint64_t); i++) {
		sdt = (void *)k_p2v_l(*(k_uint32_t *)&xsdt->entries[i]);

		if (!k_memcmp(sdt->signature, K_ACPI_MADT_SIGNATURE, 4))
			k_acpi_parse_madt((struct k_acpi_madt *)sdt);
		else if (!k_memcmp(sdt->signature, K_ACPI_HPET_SIGNATURE, 4))
			k_acpi_parse_hpet((struct k_acpi_hpet *)sdt);
	}
}

static void k_acpi_parse_rsdt(struct k_acpi_rsdt *rsdt)
{
	k_uint32_t i;
	k_error_t error;
	struct k_acpi_sdt *sdt;

	if (k_memcmp((void *)rsdt->sdt.signature, K_ACPI_RSDT_SIGNATURE, 4))
		return;

	error = k_acpi_checksum(rsdt, rsdt->sdt.length);
	if (error)
		return;

	for (i = 0; i < (rsdt->sdt.length - K_OFFSETOF(struct k_acpi_rsdt, entries)) /
		sizeof(k_uint32_t); i++) {
		sdt = (void *)k_p2v_l(*(k_uint32_t *)&rsdt->entries[i]);

		if (!k_memcmp(sdt->signature, K_ACPI_MADT_SIGNATURE, 4))
			k_acpi_parse_madt((struct k_acpi_madt *)sdt);
		else if (!k_memcmp(sdt->signature, K_ACPI_HPET_SIGNATURE, 4))
			k_acpi_parse_hpet((struct k_acpi_hpet *)sdt);
	}
}

static k_error_t k_acpi_check_rsdp(struct k_acpi_rsdp *rsdp)
{
	k_error_t error;

	error = k_acpi_checksum(rsdp, sizeof(struct k_acpi_rsdp));
	if (error)
		return error;

	if (rsdp->revision == 2) {
		error = k_acpi_checksum(rsdp, rsdp->length);
		if (error)
			return error;
	}

	return K_ERROR_NONE;
}

static unsigned long k_acpi_scan_address_range(unsigned long start, unsigned long range)
{
	unsigned long end;

	/* if (start & (0x10 - 1)
	   return; */

	end = start + range;

	while (start < end) {
		if (!k_memcmp((void *)start, K_ACPI_RSDP_SIGNATURE, 8))
			return start;

		start += 0x10;
	}

	return 0;
}

static struct k_acpi_rsdp *k_acpi_find_rsdp(void)
{
	k_error_t error;
	k_uint16_t ebda;
	struct k_acpi_rsdp *rsdp;

	rsdp = NULL;

	ebda = *(k_uint16_t *)k_p2v_l(0x40e);
	if (ebda)
		rsdp = (void *)k_acpi_scan_address_range(k_p2v_l(ebda << 4), 0x400);

	if (!rsdp)
		rsdp = (void *)k_acpi_scan_address_range(k_p2v_l(0xe0000), 0x20000);

	if (!rsdp)
		return NULL;

	error = k_acpi_check_rsdp(rsdp);
	if (error)
		return NULL;

	return rsdp;
}

void k_acpi_get_info(void *_rsdp)
{
	k_error_t error;
	struct k_acpi_rsdp *rsdp;

	if (_rsdp) {
		error = k_acpi_check_rsdp(_rsdp);
		if (error)
			rsdp = NULL;
		else
			rsdp = _rsdp;
	} else
		rsdp = k_acpi_find_rsdp();

	if (!rsdp) {
		k_acpi.found = false;
		return;
	}

	if (rsdp->revision == 0x0)
		k_acpi_parse_rsdt((struct k_acpi_rsdt *)k_p2v_l(rsdp->rsdt_address));
	else {
		if (rsdp->xsdt_address)
			k_acpi_parse_xsdt((struct k_acpi_xsdt *)k_p2v_l(rsdp->xsdt_address));
		else
			k_acpi_parse_rsdt((struct k_acpi_rsdt *)k_p2v_l(rsdp->rsdt_address));
	}

	k_acpi.found = true;
}

