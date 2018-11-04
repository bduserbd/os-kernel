#include "rtl8139.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/network/card.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/irq/irq.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("RTL8139");

struct k_rtl8139 {
	unsigned long io;
	k_uint8_t mac[6];

	unsigned int irq;

	k_uint8_t *rx_buffer;

	struct k_rtl8139 *next;
};
static struct k_rtl8139 *k_rtl8139_list = NULL;

static inline void k_rtl8139_add(struct k_rtl8139 *rtl8139)
{
	rtl8139->next = k_rtl8139_list;
	k_rtl8139_list = rtl8139;
}

static inline k_uint8_t k_rtl8139_inb(struct k_rtl8139 *rtl8139, int reg)
{
	return k_inb(rtl8139->io + reg);
}

static inline void k_rtl8139_outb(struct k_rtl8139 *rtl8139, k_uint8_t data, int reg)
{
	k_outb(data, rtl8139->io + reg);
}

static inline void k_rtl8139_outw(struct k_rtl8139 *rtl8139, k_uint16_t data, int reg)
{
	k_outw(data, rtl8139->io + reg);
}

static inline void k_rtl8139_outl(struct k_rtl8139 *rtl8139, k_uint32_t data, int reg)
{
	k_outl(data, rtl8139->io + reg);
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

static k_error_t k_rtl8139_irq_handler(unsigned int irq, void *device)
{
	k_printf("#");

	return K_ERROR_NONE_IRQ;
}

static k_error_t k_rtl8139_init(struct k_rtl8139 *rtl8139)
{
	int i;
	k_error_t error;

	k_rtl8139_outb(rtl8139, 0x0, K_RTL8139_CONFIG1);

	k_rtl8139_outb(rtl8139, K_RTL8139_CR_RST, K_RTL8139_CR);
	while (k_rtl8139_inb(rtl8139, K_RTL8139_CR) & K_RTL8139_CR_RST) ;

	for (i = 0; i < 6; i++) {
		rtl8139->mac[i] = k_rtl8139_inb(rtl8139, K_RTL8139_IDR + i);
		k_printf("%x:", rtl8139->mac[i]);
	}
	k_printf("\n");

	rtl8139->rx_buffer = k_malloc(K_KB(8) + 16 + 1500);
	if (!rtl8139->rx_buffer)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	k_printf("%x:%x", rtl8139->rx_buffer, k_v2p(rtl8139->rx_buffer));
	k_rtl8139_outl(rtl8139, k_v2p_l((unsigned long)rtl8139->rx_buffer), K_RTL8139_RBSTART);

	k_rtl8139_outw(rtl8139, K_RTL8139_IMR_ROK | K_RTL8139_IMR_TOK, K_RTL8139_IMR);

	k_rtl8139_outl(rtl8139, K_RTL8139_RCR_AAP | K_RTL8139_RCR_APM |
				K_RTL8139_RCR_AM | K_RTL8139_RCR_AB |
				K_RTL8139_RCR_WRAP |K_RTL8139_RCR_RBLEN_8KB_16BYTES,
				K_RTL8139_RCR);

	error = k_irq_request(rtl8139->irq, k_rtl8139_irq_handler, K_IRQ_FLAGS_SHARED,
			rtl8139);
	if (error)
		return error;

	k_rtl8139_outb(rtl8139, K_RTL8139_CR_TE | K_RTL8139_CR_RE, K_RTL8139_CR);

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

	io = k_pci_read_config_long(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_BAR0);
	if (!(io & 1))
		return K_ERROR_INVALID_DEVICE;

	k_pci_set_bus_mastering(index);

	rtl8139 = k_malloc(sizeof(struct k_rtl8139));
	if (!rtl8139)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	rtl8139->irq = k_pci_read_config_byte(index.bus, index.dev, index.func,
			K_PCI_CONFIG_REG_IRQ);
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

