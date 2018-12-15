#include "include/mp.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/video/print.h"

struct k_mp_info k_mp;

static k_error_t k_mp_checksum(void *ptr, int length)
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

static k_uint32_t k_mp_scan_address_range(k_uint32_t start, k_uint32_t range)
{
	k_uint32_t end;

	/* if (start & (0x10 - 1))
		   return 0; */

	end = start + range;

	while (start < end) {
		if (*(k_uint32_t *)start == K_SMP_SIGNATURE)
			return start;

		start += 0x10;
	}

	return 0;
}

static struct k_mp_floating_pointer *k_mp_get_floating_pointer(void)
{
	k_error_t error;
	k_uint16_t ebda;
	struct k_mp_floating_pointer *ptr;

	ptr = NULL;

	ebda = *(k_uint16_t *)k_p2v_l(0x40e);
	if (ebda)
		ptr = (void *)k_mp_scan_address_range(k_p2v_l(ebda << 4), 0x400);

	if (!ptr)
		(void)((ptr = (void *)k_mp_scan_address_range(k_p2v_l(639 * 0x400), 0x400)) ||
			(ptr = (void *)k_mp_scan_address_range(k_p2v_l(0xf0000), 0x10000)));

	if (!ptr)
		return NULL;

	if (ptr->configuration_type)
		return NULL;

	error = k_mp_checksum(ptr, ptr->length * 0x10);
	if (error)
		return NULL;

	return ptr;
}

static void k_mp_parse_bus_entry(struct k_mp_bus_entry *entry)
{

}

static void k_mp_parse_ioapic_entry(struct k_mp_ioapic_entry *entry)
{
	if (!k_mp.ioapic_address)
		k_mp.ioapic_address = entry->ioapic_address;
}

static void k_mp_parse_io_interrupt_entry(struct k_mp_io_interrupt_entry *entry)
{
	k_printf("%x %x %x %x %x %x\n",
			entry->interrupt_type,
			entry->io_interrupt_flags,
			entry->source_bus_id,
			entry->source_bus_irq,
			entry->destination_ioapic_id,
			entry->destination_ioapic_intin);
}

void k_mp_get_info(void)
{
	k_uint16_t i;
	struct k_mp_floating_pointer *ptr;
	struct k_mp_configuration_table *config;
	k_uint8_t *entry_type;

	k_mp.found = false;

	ptr = k_mp_get_floating_pointer();
	if (!ptr)
		return;

	config = (void *)k_p2v_l(ptr->configuration_table_pointer);
	if (config->signature != K_SMP_CONFIGURATION_TABLE_SIGNATURE)
		return;

	entry_type = (k_uint8_t *)config + sizeof(struct k_mp_configuration_table);
	for (i = 0; i < config->entry_count; i++) {
		switch (*entry_type) {
		case K_SMP_ENTRY_PROCESSOR:
			entry_type += sizeof(struct k_mp_processor_entry);

			break;

		case K_SMP_ENTRY_BUS:
			k_mp_parse_bus_entry((void *)entry_type);
			entry_type += sizeof(struct k_mp_bus_entry);

			break;

		case K_SMP_ENTRY_IOAPIC:
			k_mp_parse_ioapic_entry((void *)entry_type);
			entry_type += sizeof(struct k_mp_ioapic_entry);

			break;

		case K_SMP_ENTRY_IO_INTERRUPT:
			k_mp_parse_io_interrupt_entry((void *)entry_type);
			entry_type += sizeof(struct k_mp_io_interrupt_entry);

			break;

		case K_SMP_ENTRY_LOCAL_INTERRUPT:
			entry_type += sizeof(struct k_mp_local_interrupt_entry);

			break;

		default:
			break;
		}
	}

	k_mp.found = true;
}

