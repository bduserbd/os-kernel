#include "include/network/network.h"
#include "include/network/protocol/ethernet.h"
#include "include/string.h"

k_uint8_t k_ethernet_broadcast_address[K_MAC_LENGTH] = {
	[0 ... K_MAC_LENGTH - 1] = 0xff,
};

void k_ethernet_build_packet(struct k_network_buffer *buffer, k_uint16_t protocol,
		k_uint8_t mac_src[K_MAC_LENGTH], k_uint8_t mac_dest[K_MAC_LENGTH])
{
	struct k_ethernet_header *ethernet;

	k_network_buffer_adjust(buffer, sizeof(struct k_ethernet_header));

	ethernet = (void *)buffer->packet_start;

	k_memcpy(ethernet->mac_src, mac_src, K_MAC_LENGTH);
	k_memcpy(ethernet->mac_dest, mac_dest, K_MAC_LENGTH);
	ethernet->protocol = k_cpu_to_be16(protocol);
}

void k_ethernet_build_broadcast_packet(struct k_network_buffer *buffer, k_uint16_t protocol,
		k_uint8_t mac_src[K_MAC_LENGTH])
{
	k_ethernet_build_packet(buffer, protocol, mac_src, k_ethernet_broadcast_address);
}

