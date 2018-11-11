#include "include/network/protocol/udp.h"

static k_uint16_t k_udp_checksum(struct k_udp_header *udp)
{
	return 0;
}

void k_udp_build_packet(struct k_network_buffer *buffer, k_uint16_t payload_length,
		k_port_t src_port, k_port_t dest_port, k_ipv4_t ip)
{
	struct k_udp_header *udp;

	k_network_buffer_adjust(buffer, sizeof(struct k_udp_header));

	udp = (void *)buffer->packet_start;

	udp->src_port = k_cpu_to_be16(src_port);
	udp->dest_port = k_cpu_to_be16(dest_port);
	udp->length = k_cpu_to_be16(sizeof(struct k_udp_header) + payload_length);
	udp->checksum = k_udp_checksum(udp);

	k_ipv4_build_packet(buffer, ip);
}

