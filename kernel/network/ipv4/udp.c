#include "include/network/protocol/udp.h"

static k_uint16_t k_udp_checksum(struct k_udp_header *udp, k_ipv4_t src_ip,
		k_ipv4_t dest_ip)
{
	k_uint32_t sum;
	k_size_t length;
	const k_uint16_t *buf, *a, *b;

	length = k_be16_to_cpu(udp->length);

	buf = (void *)udp;
	a = (void *)&src_ip;
	b = (void *)&dest_ip;

	sum = 0;
	while (length > 1) {
		sum += *buf++;

		if (sum & (1 << 31))
			sum = (sum & 0xffff) + (sum >> 16);

		length -= 2;
	}

	if (length & 1)
		sum += *(k_uint8_t *)buf;

	sum += *b++; sum += *b;

	sum += *a++; sum += *a;

	sum += k_cpu_to_be16(K_IPV4_PROTOCOL_UDP);
	sum += udp->length;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (k_uint16_t)~sum;
}

void k_udp_build_packet(struct k_network_buffer *buffer, k_uint16_t payload_length,
		k_port_t src_port, k_port_t dest_port, k_ipv4_t ip)
{
	k_uint16_t length;
	struct k_udp_header *udp;

	k_network_buffer_adjust(buffer, sizeof(struct k_udp_header));

	udp = (void *)buffer->packet_start;

	udp->src_port = k_cpu_to_be16(src_port);
	udp->dest_port = k_cpu_to_be16(dest_port);

	length = sizeof(struct k_udp_header) + payload_length;
	udp->length = k_cpu_to_be16(length);

	udp->checksum = 0;
	udp->checksum = k_udp_checksum(udp, K_IPV4(0, 0, 0, 0), ip);

	k_ipv4_build_packet(buffer, length, ip);
}

