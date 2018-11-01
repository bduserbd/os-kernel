#include "rtl8139.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("RTL8139");

static k_uint8_t k_rtl8139_inb(unsigned long io, int reg)
{
	return k_inb(io + reg);
}

static k_error_t k_rtl8139_is_supported(struct k_pci_index index)
{
	k_uint16_t vendor_id, device_id;

	vendor_id = k_pci_read_config_word(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_VENDOR_ID);
	device_id = k_pci_read_config_word(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_DEVICE_ID);

	if (vendor_id != K_RTL8139_PCI_VENDOR_ID ||
			device_id != K_RTL8139_PCI_DEVICE_ID)
		return K_ERROR_NOT_FOUND;

	return K_ERROR_NONE;
}

static k_error_t k_rtl8139_init(unsigned long io)
{
	int i;

	for (i = 0; i < 6; i++)
		k_printf("%x:", k_rtl8139_inb(io, K_RTL8139_IDR + i));

	return K_ERROR_NONE;
}

static k_error_t k_rtl8139_pci_init(struct k_pci_index index)
{
	k_error_t error;
	unsigned long io;

	error = k_pci_check_device_class(index, K_PCI_BASE_CLASS_NETWORK,
			K_PCI_SUBCLASS_NETWORK_ETHERNET, -1);
	if (error)
		return error;

	error = k_rtl8139_is_supported(index);
	if (error)
		return error;

	io = k_pci_read_config_long(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_BAR0);
	if ((io & 0x1) == 0)
		return K_ERROR_INVALID_DEVICE;

	io &= ~1UL;

	k_pci_set_bus_mastering(index);

	error = k_rtl8139_init(io);
	if (error)
		return error;

	return K_ERROR_NONE;
}

K_MODULE_INIT()
{
	k_pci_iterate_devices(k_rtl8139_pci_init);
}

K_MODULE_UNINIT()
{

}

