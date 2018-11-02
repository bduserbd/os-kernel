#include "rtl8139.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/network/card.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("RTL8139");

struct k_rtl8139 {
	unsigned long io;
	k_uint8_t mac[6];
};

static inline k_uint8_t k_rtl8139_inb(struct k_rtl8139 *rtl8139, int reg)
{
	return k_inb(rtl8139->io + reg);
}

static inline void k_rtl8139_outb(struct k_rtl8139 *rtl8139, int reg, k_uint8_t data)
{
	k_outb(data, rtl8139->io + reg);
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

static k_error_t k_rtl8139_init(struct k_rtl8139 *rtl8139)
{
	int i;

	k_rtl8139_outb(rtl8139, K_RTL8139_CONFIG1, 0x0);

	k_rtl8139_outb(rtl8139, K_RTL8139_CR, K_RTL8139_CR_RST);
	while (k_rtl8139_inb(rtl8139, K_RTL8139_CR) & K_RTL8139_CR_RST) ;

	k_printf("%x ", rtl8139);

	for (i = 0; i < 6; i++) {
		rtl8139->mac[i] = k_rtl8139_inb(rtl8139, K_RTL8139_IDR + i);
		k_printf("%x:", rtl8139->mac[i]);
	}

	return K_ERROR_NONE;
}

static k_error_t k_rtl8139_pci_init(struct k_pci_index index)
{
	k_error_t error;
	unsigned long io;
	struct k_rtl8139 *rtl8139;

	error = k_pci_check_device_class(index, K_PCI_BASE_CLASS_NETWORK,
			K_PCI_SUBCLASS_NETWORK_ETHERNET, -1);
	if (error)
		return error;

	error = k_rtl8139_is_supported(index);
	if (error)
		return error;

	io = k_pci_read_config_long(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_BAR0);
	if (!(io & 1))
		return K_ERROR_INVALID_DEVICE;

	k_pci_set_bus_mastering(index);

	rtl8139 = k_malloc(sizeof(struct k_rtl8139));
	if (!rtl8139)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	rtl8139->io = io & ~1UL;

	error = k_rtl8139_init(rtl8139);
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

