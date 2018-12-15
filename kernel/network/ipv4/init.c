#include "include/network/network.h"
#include "include/network/protocol/ethernet.h"
#include "include/network/protocol/address-cache.h"
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

	int options_length = 64;

#define SIZE					\
	(sizeof(struct k_ethernet_header) +	\
	sizeof(struct k_ipv4_header) +		\
	sizeof(struct k_udp_header) +		\
	sizeof(struct k_dhcp_header) +		\
	options_length)

	buffer->start = k_malloc(SIZE);
	if (!buffer->start)
		return;

	buffer->end = buffer->start + SIZE;
	buffer->packet_start = buffer->packet_end = buffer->end;

	for (card = k_network_cards; card; card = card->next) {
		k_address_cache_create(card);

		k_address_cache_new_entry(card, K_IPV4(0, 0, 0, 0), card->hw_address);
		k_address_cache_new_entry(card, K_IPV4_BROADCAST, k_ethernet_broadcast_address);

		buffer->card = card;
		k_dhcp_build_packet(buffer, options_length);

		while ((card->ops->transmit(card, buffer) == K_ERROR_NETWORK_LINK_NOT_SET)) ;
	}
}

