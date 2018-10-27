#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("E1000");

static const struct {
	k_uint16_t vendor_id;
	k_uint16_t device_id;
} k_e1000_supported_cards[] = {
	{ 0x8086, 0x100e },
	{ 0 },
};

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
				k_e1000_supported_cards[i].device_id == device_id)
			found = true;

	if (found)
		return K_ERROR_NONE;
	else
		return K_ERROR_NOT_FOUND;
}

static k_error_t k_e1000_pci_init(struct k_pci_index index)
{
	k_uint8_t irq;
	k_error_t error;

	error = k_pci_check_device_class(index, K_PCI_BASE_CLASS_NETWORK,
			K_PCI_SUBCLASS_NETWORK_ETHERNET, -1);
	if (error)
		return error;

	error = k_e1000_is_supported(index);
	if (error)
		return error;

	irq = k_pci_read_config_byte(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_IRQ);
	k_printf("%x\n", irq);

	return K_ERROR_NONE;
}

K_MODULE_INIT()
{
	k_pci_iterate_devices(k_e1000_pci_init);
}

K_MODULE_UNINIT()
{

}

