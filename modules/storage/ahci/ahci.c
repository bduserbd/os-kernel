#include "ahci.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/time/sleep.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("Advanced Host Controller Interface");

struct k_ahci_port {
	int implemented;
};

struct k_ahci_controller {
	unsigned long dma;
	unsigned int irq;

	volatile struct k_ahci_generic_host_control *ghc;

	int number_of_ports;
	struct k_ahci_port *ports;
	volatile struct k_ahci_port_registers *port_regs;

	struct k_ahci_controller *next;
};
static struct k_ahci_controller *k_ahci_list = NULL;

static inline void k_ahci_add(struct k_ahci_controller *ahci)
{
	ahci->next = k_ahci_list;
	k_ahci_list = ahci;
}

static k_error_t k_ahci_perform_reset(struct k_ahci_controller *ahci)
{
	int timeout;

	ahci->ghc->ghc |= K_AHCI_GHC_HR;

	timeout = 1000;
	while (--timeout) {
		if ((ahci->ghc->ghc & K_AHCI_GHC_HR) == 0)
			return K_ERROR_NONE;

		k_sleep(1);
	}

	return K_ERROR_DISK_CONTROLLER_INTERNAL;
}

static k_error_t k_ahci_init(struct k_ahci_controller *ahci)
{
	int i;
	k_error_t error;

	k_memory_zone_dma_add(K_PAGE_NUMBER(ahci->dma), 1);
	ahci->ghc = (void *)k_p2v_l(ahci->dma);

	ahci->ghc->ghc |= K_AHCI_GHC_AE;

	error = k_ahci_perform_reset(ahci);
	if (error)
		return error;

	ahci->ghc->ghc |= K_AHCI_GHC_AE;

	ahci->number_of_ports = K_AHCI_CAP_NP(ahci->ghc->cap);
	ahci->ports = k_malloc(ahci->number_of_ports * sizeof(struct k_ahci_port));
	if (!ahci->ports)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	ahci->port_regs = (k_uint8_t *)ahci->ghc + 0x100;

	for (i = 0; i < ahci->number_of_ports; i++) {
		if (!(ahci->ghc->pi & (1 << i))) {
			ahci->ports[i].implemented = false;
			continue;
		}

		if (ahci->port_regs[i].cmd & (K_AHCI_PORT_CMD_ST | K_AHCI_PORT_CMD_FRE |
					K_AHCI_PORT_CMD_FR | K_AHCI_PORT_CMD_CR)) {
			if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_ST) {
				ahci->port_regs[i].cmd &= ~K_AHCI_PORT_CMD_ST;
				k_sleep(500);

				if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_CR)
					return K_ERROR_DISK_CONTROLLER_INTERNAL;
			}

			if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_FRE) {
				ahci->port_regs[i].cmd &= ~K_AHCI_PORT_CMD_FRE;
				k_sleep(500);

				if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_FR)
					return K_ERROR_DISK_CONTROLLER_INTERNAL;
			}
		}

		ahci->ports[i].implemented = true;
	}

	k_printf("Ports: %u\n", ahci->number_of_ports);

	return K_ERROR_NONE;
}

static k_error_t k_ahci_pci_init(struct k_pci_index index)
{
	k_uint8_t irq;
	k_error_t error;
	k_uint32_t bar5;
	struct k_ahci_controller *ahci;

	error = k_pci_check_device_class(index, K_PCI_BASE_CLASS_STORAGE,
			K_PCI_SUBCLASS_STORAGE_SATA, K_PCI_PROG_IF_STORAGE_SATA_AHCI);
	if (error)
		return error;

	bar5 = k_pci_read_config_long(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_BAR5);
	if (bar5 & ((1 << 0) | (1 << 1) | (1 << 2)))
		return K_ERROR_DISK_CONTROLLER_INTERNAL;

	ahci = k_malloc(sizeof(struct k_ahci_controller));
	if (!ahci)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	irq = k_pci_read_config_byte(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_IRQ);

	ahci->dma = bar5 & ~0xfffUL;
	ahci->irq = irq;

	error = k_ahci_init(ahci);
	if (error)
		return error;

	k_ahci_add(ahci);

	return K_ERROR_NONE;
}

K_MODULE_INIT()
{
	k_pci_iterate_devices(k_ahci_pci_init);
}

K_MODULE_UNINIT()
{

}

