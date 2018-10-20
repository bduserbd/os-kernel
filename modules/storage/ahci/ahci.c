#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("Advanced Host Controller Interface");

static k_error_t k_ahci_pci_init(struct k_pci_index i)
{
	k_error_t error;

	error = k_pci_check_device_class(i, K_PCI_BASE_CLASS_STORAGE,
			K_PCI_SUBCLASS_STORAGE_SATA, K_PCI_PROG_IF_STORAGE_SATA_AHCI);
	if (error)
		return error;

	k_printf("%d %d %d\n", i.bus, i.dev, i.func);

	return K_ERROR_NONE;
}

K_MODULE_INIT()
{
	k_pci_iterate_devices(k_ahci_pci_init);
}

K_MODULE_UNINIT()
{

}

