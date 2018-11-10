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

	k_memset(buffer->start, 'A', sizeof(struct k_ethernet_header));

	for (int j = 0; j < 10; j++)
		for (int i = 0; i < 100000000; i++)
			asm volatile("nop");

	k_printf("Here");

	for (card = k_network_cards; card; card = card->next) {
		card->ops->transmit(card, buffer);
	}
}

