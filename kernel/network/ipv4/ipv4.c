#include "include/network/network.h"
#include "include/network/protocol/ipv4.h"
#include "include/network/protocol/ethernet.h"

static k_uint16_t k_ipv4_checksum(struct k_ipv4_header *ipv4)
{
	return 0;
}

void k_ipv4_build_packet(struct k_network_buffer *buffer, k_uint8_t mac[K_MAC_LENGTH])
{
	struct k_ipv4_header *ipv4;

	k_network_buffer_adjust(buffer, sizeof(struct k_ipv4_header));

	ipv4 = (void *)buffer->packet_start;

	ipv4->version = K_IPV4_VERSION;
	ipv4->ihl = sizeof(struct k_ipv4_header) >> 2;
	ipv4->tos = K_IPV4_TOS_IMMEDIATE | K_IPV4_TOS_R;
	ipv4->total_length = 0;
	ipv4->id = 0;
	ipv4->fragment_offset = 0;
	ipv4->ttl = K_IPV4_TTL;
	ipv4->protocol = K_IPV4_PROTOCOL_UDP;
	ipv4->checksum = 0;
	ipv4->src_ip = k_cpu_to_be32(0x0);
	ipv4->src_ip = k_cpu_to_be32(0xffffffff);

	ipv4->checksum = k_ipv4_checksum(ipv4);

	k_ethernet_build_broadcast_packet(buffer, K_ETHERNET_PROTOCOL_IP, mac);
}

