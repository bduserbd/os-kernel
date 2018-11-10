#include "include/network/network.h"
#include "include/network/protocol/ethernet.h"
#include "include/network/protocol/ipv4.h"
#include "include/network/protocol/dhcp.h"
#include "include/network/protocol/udp.h"
#include "include/mm/mm.h"
#include "include/string.h"
#include "include/video/print.h"

void k_ipv4_init(void)
{
	struct k_network_card *card;
	struct k_network_buffer *buffer;

	buffer = k_malloc(sizeof(struct k_network_buffer));
	if (!buffer)
		return;

	buffer->start = k_malloc(sizeof(struct k_ethernet_header));
	if (!buffer->start)
		return;

	buffer->end = buffer->start + sizeof(struct k_ethernet_header);

	for (int n = 0; n < 2; n++) {
		for (int j = 0; j < 10; j++)
			for (int i = 0; i < 100000000; i++)
				asm volatile("nop");

		for (card = k_network_cards; card; card = card->next) {
			k_ethernet_build_broadcast_packet(buffer, K_ETHERNET_PROTOCOL_IP,
					card->hw_address);

			card->ops->transmit(card, buffer);
		}
	}
}

