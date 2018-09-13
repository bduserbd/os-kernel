#include "include/acpi/acpi.h"
#include "include/string.h"

void puts(const char *);
void puthex(k_uint64_t);

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

static void k_acpi_parse_madt(struct k_acpi_madt *madt)
{
	k_uint8_t *type, length;

	/* Checksum. */
	type = &madt->entries[0];
	length = *(k_uint8_t *)(type + 1);

	type += length; length = *(k_uint8_t *)(type + 1); puthex(*type); puthex(length); puts("|");
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
		sdt = (void *)*(k_uint32_t *)&rsdt->entries[i];

		if (!k_memcmp(sdt->signature, K_ACPI_MADT_SIGNATURE, 4))
			k_acpi_parse_madt((void *)sdt);
	}
}

static k_error_t k_acpi_check_rsdp(struct k_acpi_rsdp *rsdp)
{
	k_error_t error;

	error = k_acpi_checksum(rsdp, sizeof(struct k_acpi_rsdp));
	if (error)
		return error;

	if (rsdp->revision == 0x2) {
		error = k_acpi_checksum(rsdp, rsdp->length);
		if (error)
			return error;
	}

	return K_ERROR_NONE;
}

static k_uint32_t k_acpi_scan_address_range(k_uint32_t start, k_uint32_t range)
{
	k_uint32_t end;

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

	ebda = *(k_uint16_t *)0x40e;
	if (ebda)
		rsdp = (void *)k_acpi_scan_address_range(ebda << 4, 0x400);

	if (!rsdp)
		rsdp = (void *)k_acpi_scan_address_range(0xe0000, 0x20000);

	if (!rsdp)
		return NULL;

	error = k_acpi_check_rsdp(rsdp);
	if (error)
		return NULL;

	return rsdp;
}

void k_acpi_get_info(void)
{
	struct k_acpi_rsdp *rsdp;

	rsdp = k_acpi_find_rsdp();
	if (!rsdp)
		return;

	if (rsdp->revision == 0x0)
		k_acpi_parse_rsdt((struct k_acpi_rsdt *)rsdp->rsdt_address);
	else {
		//if (rsdp->xsdt_address)
		//	k_acpi_parse_xsdt((struct k_acpi_xsdt *)rsdp->xsdt_address);
		//else
		//	k_acpi_parse_rsdt((struct k_acpi_rsdt *)rsdp->rsdt_address);
	}
}

