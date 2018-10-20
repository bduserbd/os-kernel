#include "include/pci/pci.h"
#include "include/modules/export-symbol.h"

void k_pci_iterate_devices(pci_device_hook_t hook)
{
	k_uint8_t header;
	k_uint16_t vendor;
	struct k_pci_index i;

	for (i.bus = 0; i.bus < K_PCI_NBUSES; i.bus++) {
		for (i.dev = 0; i.dev < K_PCI_NDEVICES; i.dev++) {
			for (i.func = 0; i.func < K_PCI_NFUNCTIONS; i.func++) {
				vendor = k_pci_read_config_word(i.bus, i.dev, i.func,
						K_PCI_CONFIG_REG_VENDOR_ID);

				/* Invalid device */
				if (vendor == 0xffff)
					continue;

				if (hook)
					hook(i);

				if (i.func == 0) {
					header = k_pci_read_config_byte(i.bus, i.dev, i.func,
							K_PCI_CONFIG_REG_HEADER_TYPE);

					/* If the 7th bit is 0, the device has a single function */
					if (!(header & 0x80))
						break;
				}
			}
		}
	}
}
K_EXPORT_FUNC(k_pci_iterate_devices);

k_error_t k_pci_check_device_class(struct k_pci_index i,
		int class, int subclass, int interface)
{
	if (class != -1)
		if (class != k_pci_read_config_byte(i.bus, i.dev, i.func,
					K_PCI_CONFIG_REG_BASECLASS))
			return K_ERROR_FAILURE;

	if (subclass != -1)
		if (subclass != k_pci_read_config_byte(i.bus, i.dev, i.func,
					K_PCI_CONFIG_REG_SUBCLASS))
			return K_ERROR_FAILURE;

	if (interface != -1)
		if (interface != k_pci_read_config_byte(i.bus, i.dev, i.func,
					K_PCI_CONFIG_REG_PROG_IF))
			return K_ERROR_FAILURE;

	return K_ERROR_NONE;
}
K_EXPORT_FUNC(k_pci_check_device_class);

