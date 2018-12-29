#include "ahci.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/time/sleep.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("Advanced Host Controller Interface");

struct k_ahci_port {
	int implemented;

	volatile struct k_ahci_port_registers *regs;

	volatile struct k_ahci_command_header *command_list;

	volatile struct k_ahci_command_table **command_table;

	volatile struct k_ahci_received_fis *received_fis;
};

struct k_ahci_controller {
	unsigned long dma;
	unsigned int irq;

	bool address_64bit;

	volatile struct k_ahci_generic_host_control *ghc;

	int number_of_ports;
	struct k_ahci_port *ports;
	volatile struct k_ahci_port_registers *port_regs;

	int command_slots;

	struct k_ahci_controller *next;
};
static struct k_ahci_controller *k_ahci_list = NULL;

static struct k_cache *k_ahci_command_list_cache = NULL;
static struct k_cache *k_ahci_command_table_cache = NULL;
static struct k_cache *k_ahci_received_fis_cache = NULL;

static k_error_t k_ahci_caches_init(void)
{
	k_ahci_command_list_cache = k_cache_create("AHCI Command List",
			32 * sizeof(struct k_ahci_command_header), 0x400, 0x0);
	if (!k_ahci_command_list_cache)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	k_ahci_command_table_cache = k_cache_create("AHCI Command Table",
			sizeof(struct k_ahci_command_table), 0x80, 0x0);
	if (!k_ahci_command_table_cache)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	k_ahci_received_fis_cache = k_cache_create("AHCI Received FIS",
			sizeof(struct k_ahci_received_fis), 0x100, 0x0);
	if (!k_ahci_received_fis_cache)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	return K_ERROR_NONE;
}

static inline void *k_ahci_command_list_alloc(void)
{
	return k_cache_alloc(k_ahci_command_list_cache);
}

static inline void *k_ahci_command_table_alloc(void)
{
	return k_cache_alloc(k_ahci_command_table_cache);
}

static inline void *k_ahci_received_fis_alloc(void)
{
	return k_cache_alloc(k_ahci_received_fis_cache);
}

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

static k_error_t k_ahci_command_list_init(struct k_ahci_controller *ahci, int port)
{
	unsigned long address;
	volatile struct k_ahci_command_header *command_list;

	command_list = k_ahci_command_list_alloc();
	if (!command_list)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	address = k_v2p_l((unsigned long)command_list);

	ahci->port_regs[port].clb = (k_uint32_t)address;
	if (ahci->address_64bit)
#ifdef K_BITS_32
		ahci->port_regs[port].clbu = 0x0;
#elif K_BITS_64
		ahci->port_regs[port].clbu = (k_uint32_t)(address >> 32);
#endif

	ahci->ports[port].command_list = command_list;

	return K_ERROR_NONE;
}

static k_error_t k_ahci_command_table_init(struct k_ahci_controller *ahci, int port)
{
	int i;
	unsigned long address;
	volatile struct k_ahci_command_table **command_table, *table;

	command_table = k_malloc(ahci->command_slots * sizeof(volatile struct k_ahci_command_table *));
	if (!command_table)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	for (i = 0; i < ahci->command_slots; i++) {
		table = k_ahci_command_table_alloc();
		if (!table)
			return K_ERROR_MEMORY_ALLOCATION_FAILED;

		command_table[i] = table;

		address = k_v2p_l((unsigned long)table);

		ahci->ports[port].command_list[i].ctba = (k_uint32_t)address;
		if (ahci->address_64bit)
#ifdef K_BITS_32
			ahci->ports[port].command_list[i].ctbau = 0x0;
#elif K_BITS_64
			ahci->ports[port].command_list[i].ctbau = (k_uint32_t)(address >> 32);
#endif
	}

	ahci->ports[port].command_table = command_table;

	return K_ERROR_NONE;
}

static k_error_t k_ahci_received_fis_init(struct k_ahci_controller *ahci, int port)
{
	unsigned long address;
	volatile struct k_ahci_received_fis *received_fis;

	received_fis = k_ahci_received_fis_alloc();
	if (!received_fis)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	address = k_v2p_l((unsigned long)received_fis);

	ahci->port_regs[port].fb = (k_uint32_t)address;
	if (ahci->address_64bit)
#ifdef K_BITS_32
		ahci->port_regs[port].fbu = 0x0;
#elif K_BITS_64
		ahci->port_regs[port].fbu = (k_uint32_t)(address >> 32);
#endif

	ahci->ports[port].received_fis = received_fis;

	return K_ERROR_NONE;
}

static k_error_t k_ahci_detect_devices(struct k_ahci_controller *ahci)
{
	int i;

	for (i = 0; i < ahci->number_of_ports; i++) {
		if (!ahci->ports[i].implemented)
			continue;

		if ((ahci->port_regs[i].tfd & (K_AHCI_PORT_TFD_STS_DRQ | K_AHCI_PORT_TFD_STS_BSY)) ||
				K_AHCI_PORT_SSTS_DET(ahci->port_regs[i].ssts) !=
				K_AHCI_PORT_SSTS_DET_PRESENT_PHYS)
			continue;

		ahci->port_regs[i].cmd |= K_AHCI_PORT_CMD_ST;

		k_printf("AHCI Device on port: %u ", i);
	}
	k_printf("\n");

	return K_ERROR_NONE;
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

	ahci->port_regs = (void *)((k_uint8_t *)ahci->ghc + 0x100);

	for (i = 0; i < ahci->number_of_ports; i++) {
		ahci->ports[i].implemented = false;

		if (!(ahci->ghc->pi & (1 << i)))
			continue;

		if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_ST) {
			ahci->port_regs[i].cmd &= ~K_AHCI_PORT_CMD_ST;
			k_sleep(500);

			if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_CR)
				continue;
		}

		if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_FRE) {
			ahci->port_regs[i].cmd &= ~K_AHCI_PORT_CMD_FRE;
			k_sleep(500);

			if (ahci->port_regs[i].cmd & K_AHCI_PORT_CMD_FR)
				continue;
		}

		ahci->ports[i].implemented = true;
	}

	ahci->command_slots = K_AHCI_CAP_NCS(ahci->ghc->cap);

	if (ahci->ghc->cap & K_AHCI_CAP_S64A)
		ahci->address_64bit = true;
	else
		ahci->address_64bit = false;

	for (i = 0; i < ahci->number_of_ports; i++) {
		if (!ahci->ports[i].implemented)
			continue;

		ahci->ports[i].regs = &ahci->port_regs[i];

		error = k_ahci_command_list_init(ahci, i);
		if (error)
			return error;

		error = k_ahci_command_table_init(ahci, i);
		if (error)
			return error;

		error = k_ahci_received_fis_init(ahci, i);
		if (error)
			return error;

		ahci->port_regs[i].cmd |= K_AHCI_PORT_CMD_FRE;
	}

	for (i = 0; i < ahci->number_of_ports; i++)
		if (ahci->ports[i].implemented)
			ahci->port_regs[i].serr = ahci->port_regs[i].serr;

	error = k_ahci_detect_devices(ahci);
	if (error)
		return error;

	k_printf("Ports: %u Command slots: %u\n", ahci->number_of_ports, ahci->command_slots);

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
	k_error_t error;

	error = k_ahci_caches_init();
	if (error)
		return;

	k_pci_iterate_devices(k_ahci_pci_init);
}

K_MODULE_UNINIT()
{

}

