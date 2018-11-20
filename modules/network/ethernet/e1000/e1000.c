#include "e1000.h"
#include "kernel/include/network/network.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/irq/irq.h"
#include "kernel/include/string.h"

K_MODULE_NAME("Intel(R) PRO/1000 (E1000)");

static const struct {
	k_uint16_t vendor_id;
	k_uint16_t device_id;
} k_e1000_pci_supported_cards[] = {
	{ 0x8086, 0x100e },
	{ 0 },
};

#define K_E1000_BUFFER_SIZE	0x1000

#define K_E1000_RX_RING_SIZE	0x1000
#define K_E1000_RX_RING_ENTRIES	(K_E1000_RX_RING_SIZE / sizeof(struct k_e1000_rdesc))

#define K_E1000_TX_RING_SIZE	0x1000
#define K_E1000_TX_RING_ENTRIES	(K_E1000_TX_RING_SIZE / sizeof(struct k_e1000_tdesc))

struct k_e1000 {
	unsigned long dma, virtual;
	unsigned int irq;

	k_uint8_t mac[6];

	int link_up;

	int receive_index;
	volatile struct k_e1000_rdesc *rx_ring;

	int transmit_index;
	volatile struct k_e1000_tdesc *tx_ring;

	struct k_network_card *card;

	struct k_e1000 *next;
};
static struct k_e1000 *k_e1000_list = NULL;

static inline void k_e1000_add(struct k_e1000 *e1000)
{
	e1000->next = k_e1000_list;
	k_e1000_list = e1000;
}

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
	for (i = 0; k_e1000_pci_supported_cards[i].vendor_id; i++)
		if (k_e1000_pci_supported_cards[i].vendor_id == vendor_id &&
				k_e1000_pci_supported_cards[i].device_id == device_id) {
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

static k_error_t k_e1000_transmit(struct k_network_card *card, struct k_network_buffer *buffer)
{
	struct k_e1000 *e1000;
	volatile struct k_e1000_tdesc *tdesc;

	e1000 = card->data;

	if (!e1000->link_up)
		return K_ERROR_NETWORK_LINK_NOT_SET;

	tdesc = &e1000->tx_ring[e1000->transmit_index];

	tdesc->buffer = k_v2p_l((unsigned long)buffer->start);
	tdesc->length = buffer->end - buffer->start;
	tdesc->cmd = K_E1000_TDESC_CMD_EOP | K_E1000_TDESC_CMD_IFCS | K_E1000_TDESC_CMD_RS;
	tdesc->status = 0;

	e1000->transmit_index = (e1000->transmit_index + 1) % K_E1000_TX_RING_ENTRIES;

	k_e1000_set_reg(e1000, K_E1000_TDT, e1000->transmit_index);

	return K_ERROR_NONE;
}

static k_error_t k_e1000_handle_receive(struct k_e1000 *e1000)
{
	k_error_t error;

	while (e1000->rx_ring[e1000->receive_index].status & K_E1000_RDESC_STATUS_DD) {
		struct k_network_buffer *buffer;

		e1000->rx_ring[e1000->receive_index].status = 0;

		buffer = k_malloc(sizeof(struct k_network_buffer));
		if (!buffer)
			return K_ERROR_MEMORY_ALLOCATION_FAILED;

		buffer->card = e1000->card;

		buffer->start = (void *)k_p2v_l((unsigned long)e1000->rx_ring[e1000->receive_index].buffer);
		buffer->end = buffer->start + e1000->rx_ring[e1000->receive_index].length;

		k_printf("%x", buffer->end - buffer->start);

		error = k_network_rx(buffer);
		if (error)
			return error;

		k_e1000_set_reg(e1000, K_E1000_RDT, e1000->receive_index);
		e1000->receive_index = (e1000->receive_index + 1) % K_E1000_RX_RING_ENTRIES;
	}

	return K_ERROR_NONE;
}

static k_error_t k_e1000_irq_handler(unsigned int irq, void *device)
{
	k_uint32_t icr;
	struct k_e1000 *e1000;
	k_error_t error;

	e1000 = device;

	icr = k_e1000_get_reg(e1000, K_E1000_ICR);

	if (icr & K_E1000_ICR_LCS)
		e1000->link_up = true;

	if (icr & K_E1000_ICR_RXT0) {
		error = k_e1000_handle_receive(e1000);
		if (error)
			return error;
	}

	return K_ERROR_NONE_IRQ;
}

static void k_e1000_get_mac_address(struct k_e1000 *e1000)
{
	k_uint16_t word;

	word = k_e1000_eeprom_get_reg(e1000, K_E1000_ETHERNET_ADDRESS0);
	*(k_uint8_t *)&e1000->mac[0] = word & 0xff;
	*(k_uint8_t *)&e1000->mac[1] = (word >> 8) & 0xff;

	word = k_e1000_eeprom_get_reg(e1000, K_E1000_ETHERNET_ADDRESS1);
	*(k_uint8_t *)&e1000->mac[2] = word & 0xff;
	*(k_uint8_t *)&e1000->mac[3] = (word >> 8) & 0xff;

	word = k_e1000_eeprom_get_reg(e1000, K_E1000_ETHERNET_ADDRESS2);
	*(k_uint8_t *)&e1000->mac[4] = word & 0xff;
	*(k_uint8_t *)&e1000->mac[5] = (word >> 8) & 0xff;
}

static k_error_t k_e1000_receive_init(struct k_e1000 *e1000)
{
	int i;

	e1000->rx_ring = k_buddy_alloc(K_E1000_RX_RING_SIZE);
	if (!e1000->rx_ring)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	for (i = 0; i < K_E1000_RX_RING_ENTRIES; i++) {
		e1000->rx_ring[i].buffer = k_v2p_l((unsigned long)k_buddy_alloc(K_E1000_BUFFER_SIZE));
		e1000->rx_ring[i].status = 0;
	}

	k_e1000_set_reg(e1000, K_E1000_RDBAL, k_v2p_l((unsigned long)e1000->rx_ring));
	k_e1000_set_reg(e1000, K_E1000_RDLEN, K_E1000_RX_RING_SIZE);
	k_e1000_set_reg(e1000, K_E1000_RDH, 0);
	k_e1000_set_reg(e1000, K_E1000_RDT, K_E1000_RX_RING_ENTRIES - 1);

	k_e1000_set_reg(e1000, K_E1000_RCTL, K_E1000_RCTL_MPE |
						K_E1000_RCTL_LBM | K_E1000_RCTL_RDMTS_1_8 |
						K_E1000_RCTL_BAM | K_E1000_RCTL_BSIZE_4096);

	e1000->receive_index = 0;

	return K_ERROR_NONE;
}

static k_error_t k_e1000_transmit_init(struct k_e1000 *e1000)
{
	int i;

	e1000->tx_ring = k_buddy_alloc(K_E1000_TX_RING_SIZE);
	if (!e1000->tx_ring)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	for (i = 0; i < K_E1000_TX_RING_ENTRIES; i++) {
		e1000->tx_ring[i].buffer = 0;
		e1000->tx_ring[i].cmd = 0;
	}

	k_e1000_set_reg(e1000, K_E1000_TDBAL, k_v2p_l((unsigned long)e1000->tx_ring));
	k_e1000_set_reg(e1000, K_E1000_TDLEN, K_E1000_TX_RING_SIZE);
	k_e1000_set_reg(e1000, K_E1000_TDH, 0);
	k_e1000_set_reg(e1000, K_E1000_TDT, 0);

	k_e1000_set_reg(e1000, K_E1000_TCTL, K_E1000_TCTL_EN | K_E1000_TCTL_PSP |
						K_E1000_TCTL_CT | K_E1000_TCTL_COLD_FULL_DUPLEX);

	e1000->transmit_index = 0;

	return K_ERROR_NONE;
}

static k_error_t k_e1000_set_link(struct k_e1000 *e1000)
{
	e1000->link_up = false;

	k_e1000_set_reg(e1000, K_E1000_CTRL, K_E1000_CTRL_ASDE | K_E1000_CTRL_SLU);

	k_e1000_set_reg(e1000, K_E1000_IMS, K_E1000_IMS_TXDW | K_E1000_IMS_LSC |
						K_E1000_IMS_RXSEQ | K_E1000_IMS_RXDMT0 |
						K_E1000_IMS_RXO | K_E1000_IMS_RXT0 |
						K_E1000_IMS_MDAC | K_E1000_IMS_PHYINT |
						K_E1000_IMS_GPI);

	k_e1000_get_reg(e1000, K_E1000_ICR);

	return K_ERROR_NONE;
}

static k_error_t k_e1000_init(struct k_e1000 *e1000)
{
	k_error_t error;

	/* Number of page frames the register address space requires. */
	k_memory_zone_dma_add(e1000->dma >> 12, 4);
	e1000->virtual = k_p2v_l(e1000->dma);

	if (!k_e1000_eeprom_valid(e1000))
		return K_ERROR_BAD_CHECKSUM;

	k_e1000_get_mac_address(e1000);

	error = k_e1000_receive_init(e1000);
	if (error)
		return error;

	error = k_e1000_transmit_init(e1000);
	if (error)
		return error;

	error = k_e1000_set_link(e1000);
	if (error)
		return error;

	k_e1000_set_reg(e1000, K_E1000_RCTL, K_E1000_RCTL_EN);

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

	e1000 = k_malloc(sizeof(struct k_e1000));
	if (!e1000)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	k_pci_set_bus_mastering(index);

	irq = k_pci_read_config_byte(index.bus, index.dev, index.func, K_PCI_CONFIG_REG_IRQ);

	e1000->dma = bar0 & ~0xf;
	e1000->irq = irq;

	error = k_e1000_init(e1000);
	if (error)
		return error;

	k_e1000_add(e1000);

	error = k_irq_request(e1000->irq, k_e1000_irq_handler, K_IRQ_FLAGS_SHARED, e1000);
	if (error)
		return error;

	return K_ERROR_NONE;
}

static struct k_network_card_operations k_e1000_ops = {
	.transmit = k_e1000_transmit,
};

K_MODULE_INIT()
{
	struct k_e1000 *e1000;
	struct k_network_packet *packet;

	k_pci_iterate_devices(k_e1000_pci_init);

	packet = k_network_packet_get(K_NETWORK_PACKET_ETHERNET);
	if (!packet)
		return;

	for (e1000 = k_e1000_list; e1000; e1000 = e1000->next) {
		struct k_network_card *card = k_malloc(sizeof(struct k_network_card));
		if (!card)
			return;

		k_memcpy(card->hw_address, e1000->mac, 6);
		card->ops = &k_e1000_ops;
		card->data = e1000;
		card->packet = packet;

		e1000->card = card;

		k_network_card_register(card);
	}
}

K_MODULE_UNINIT()
{

}

