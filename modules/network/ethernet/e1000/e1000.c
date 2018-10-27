#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("E1000");

static k_error_t k_e1000_pci_init(struct k_pci_index i)
{
	k_uint8_t irq;
	k_error_t error;

	error = k_pci_check_device_class(i, K_PCI_BASE_CLASS_NETWORK,
			K_PCI_SUBCLASS_NETWORK_ETHERNET, -1);
	if (error)
		return error;

	irq = k_pci_read_config_byte(i.bus, i.dev, i.func, K_PCI_CONFIG_REG_IRQ);

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

