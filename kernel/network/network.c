#include "include/network/card.h"
#include "include/irq/late-irq.h"
#include "include/modules/export-symbol.h"

struct k_network_card *k_network_cards = NULL;

static struct k_network_buffer *k_network_rx_packets = NULL;

void k_ipv4_init(void);

k_error_t k_network_rx(struct k_network_buffer *buffer)
{
	k_error_t error;

	buffer->next = k_network_rx_packets;
	k_network_rx_packets = buffer;

	error = k_late_irq_raise(0);
	if (error)
		return error;

	return K_ERROR_NONE;
}
K_EXPORT_FUNC(k_network_rx);

static k_error_t k_network_rx_handler(void)
{
	struct k_network_buffer *buffer;

	asm volatile("cli");

	for (buffer = k_network_rx_packets; buffer; buffer = buffer->next) {
		buffer->card->packet->callback(buffer);
	}

	asm volatile("sti");

	return K_ERROR_NONE;
}

void k_network_init(void)
{
	k_error_t error;

	error = k_late_irq_request(0, k_network_rx_handler);
	if (error)
		return;

	k_ipv4_init();
}

void k_network_card_register(struct k_network_card *card)
{
	card->next = k_network_cards;
	k_network_cards = card;
}
K_EXPORT_FUNC(k_network_card_register);

void k_network_card_unregister(struct k_network_card *card)
{

}
K_EXPORT_FUNC(k_network_card_unregister);

