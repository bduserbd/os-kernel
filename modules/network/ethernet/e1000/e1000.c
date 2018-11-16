#include "e1000.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"
#include "kernel/include/mm/mm.h"

K_MODULE_NAME("E1000");

static const struct {
	k_uint16_t vendor_id;
	k_uint16_t device_id;
} k_e1000_supported_cards[] = {
	{ 0x8086, 0x100e },
	{ 0 },
};

struct k_e1000 {
	unsigned long dma, virtual;
	unsigned int irq;

	k_uint8_t mac[6];

	struct k_e1000 *next;
};

static inline void k_e1000_set_reg(struct k_e1000 *e1000, int reg, k_uint32_t data)
{
	*(volatile k_uint32_t *)(e1000->virtual + reg) = data;
}

static inline k_uint32_t k_e1000_get_reg(struct k_e1000 *e1000, int reg)
{
	return *(volatile k_uint32_t *)(e1000->virtual + reg);
}

static k_uint16_t k_e1000_eeprom_get_reg(struct k_e1000 *e1000, int reg)
{
	k_uint32_t eerd;

	k_e1000_set_reg(e1000, K_E1000_EERD, K_E1000_EERD_START | K_E1000_EERD_ADDR(reg));

	while (!((eerd = k_e1000_get_reg(e1000, K_E1000_EERD)) & K_E1000_EERD_DONE)) ;

	return K_E1000_EERD_DATA(eerd);
}

static k_error_t k_e1000_is_supported(struct k_pci_index index)
{
	int i;
	int found;
	k_uint16_t vendor_id, device_id;

	vendor_id = k_pci_read_config_word(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_VENDOR_ID);
	device_id = k_pci_read_config_word(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_DEVICE_ID);

	found = false;
	for (i = 0; k_e1000_supported_cards[i].vendor_id; i++)
		if (k_e1000_supported_cards[i].vendor_id == vendor_id &&
				k_e1000_supported_cards[i].device_id == device_id) {
			found = true;
			break;
		}

	if (found)
		return K_ERROR_NONE;
	else
		return K_ERROR_NOT_FOUND;
}

static int k_e1000_eeprom_valid(struct k_e1000 *e1000)
{
	return K_E1000_EEPROM_INIT_CONTROL1_SIGNATURE_VALID(k_e1000_eeprom_get_reg(e1000,
			K_E1000_EEPROM_INIT_CONTROL1));
}

static void k_e1000_get_mac_address(struct k_e1000 *e1000)
{
	*(k_uint16_t *)&e1000->mac[0] = (k_e1000_eeprom_get_reg(e1000, K_E1000_ETHERNET_ADDRESS0));
	*(k_uint16_t *)&e1000->mac[2] = (k_e1000_eeprom_get_reg(e1000, K_E1000_ETHERNET_ADDRESS1));
	*(k_uint16_t *)&e1000->mac[4] = (k_e1000_eeprom_get_reg(e1000, K_E1000_ETHERNET_ADDRESS2));

	for (int i = 0; i < 6; i++)
		k_printf("%x", e1000->mac[i]);
}

static k_error_t k_e1000_init(struct k_e1000 *e1000)
{
	k_memory_zone_dma_add(e1000->dma >> 12, 1);
	e1000->virtual = k_p2v_l(e1000->dma);

	if (!k_e1000_eeprom_valid(e1000))
		return K_ERROR_BAD_CHECKSUM;

	k_e1000_get_mac_address(e1000);

	return K_ERROR_NONE;
}

static k_error_t k_e1000_pci_init(struct k_pci_index index)
{
	k_uint8_t irq;
	k_error_t error;
	k_uint32_t bar0;
	struct k_e1000 *e1000;

	error = k_pci_check_device_class(index, K_PCI_BASE_CLASS_NETWORK,
			K_PCI_SUBCLASS_NETWORK_ETHERNET, -1);
	if (error)
		return error;

	error = k_e1000_is_supported(index);
	if (error)
		return error;

	bar0 = k_pci_read_config_long(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_BAR0);
	if (bar0 & ((1 << 0) | (2 << 1)))
		return K_ERROR_INVALID_DEVICE;

	irq = k_pci_read_config_byte(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_IRQ);

	e1000 = k_malloc(sizeof(struct k_e1000));
	if (!e1000)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	e1000->dma = bar0 & ~0xf;
	e1000->irq = irq;

	error = k_e1000_init(e1000);
	if (error)
		return error;

	return K_ERROR_NONE;
}

K_MODULE_INIT()
{
	k_pci_iterate_devices(k_e1000_pci_init);
}

K_MODULE_UNINIT()
{

}

