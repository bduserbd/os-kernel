#include "include/network/protocol/address-cache.h"

static k_uint16_t k_ipv4_checksum(struct k_ipv4_header *ipv4)
{
	k_uint32_t sum;
	k_size_t length;
	const k_uint16_t *buf;

	length = ipv4->ihl << 2;

	buf = (void *)ipv4;

	sum = 0;
	while (length > 1) {
		sum += *buf++;

		if (sum & (1 << 31))
			sum = (sum & 0xffff) + (sum >> 16);

		length -= 2;
	}

	if (length & 1)
		sum += *(k_uint8_t *)buf;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (k_uint16_t)~sum;
}

void k_ipv4_build_packet(struct k_network_buffer *buffer, k_uint16_t payload_length,
		k_ipv4_t ip)
{
	k_uint8_t mac[K_MAC_LENGTH];
	struct k_ipv4_header *ipv4;

	k_network_buffer_adjust(buffer, sizeof(struct k_ipv4_header));

	ipv4 = (void *)buffer->packet_start;

	ipv4->version = K_IPV4_VERSION;
	ipv4->ihl = sizeof(struct k_ipv4_header) >> 2;
	ipv4->tos = K_IPV4_TOS_T;
	ipv4->total_length = k_cpu_to_be16(sizeof(struct k_ipv4_header) + payload_length);
	ipv4->id = 0;
	ipv4->fragment_offset = 0;
	ipv4->ttl = K_IPV4_TTL;
	ipv4->protocol = K_IPV4_PROTOCOL_UDP;
	ipv4->checksum = 0;
	ipv4->src_ip = K_IPV4(0, 0, 0, 0);
	ipv4->dest_ip = ip;

	ipv4->checksum = k_ipv4_checksum(ipv4);

	k_address_cache_resolve(buffer->card, ipv4->src_ip, mac);

	k_ethernet_build_broadcast_packet(buffer, K_ETHERNET_PROTOCOL_IP, mac);
}

