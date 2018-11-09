#include "include/network/buffer.h"
#include "include/network/protocol/ethernet.h"
#include "include/network/protocol/ipv4.h"
#include "include/network/protocol/dhcp.h"
#include "include/network/protocol/udp.h"
#include "include/mm/mm.h"

void k_ipv4_init(void)
{
	struct k_network_buffer *buffer;

	buffer = k_malloc(sizeof(struct k_network_buffer));
	if (!buffer)
		return;

	buffer->length = sizeof(struct k_ethernet_header) + sizeof(struct k_ipv4_header) +
			sizeof(struct k_udp_header) + sizeof(struct k_dhcp_header);
}

