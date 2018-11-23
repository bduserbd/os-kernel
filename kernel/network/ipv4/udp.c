#include "include/network/protocol/udp.h"
#include "include/network/ipv4/ipv4.h"

static k_uint16_t k_udp_checksum(struct k_udp_header *udp, k_ipv4_t ip_src, k_ipv4_t ip_dest)
{
	k_uint32_t sum;
	k_size_t length;
	const k_uint16_t *buf, *a, *b;

	length = k_be16_to_cpu(udp->length);

	buf = (void *)udp;
	a = (void *)&ip_src;
	b = (void *)&ip_dest;

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
		k_port_t port_src, k_port_t port_dest, k_ipv4_t ip)
{
	k_uint16_t length;
	struct k_udp_header *udp;

	k_network_buffer_adjust(buffer, sizeof(struct k_udp_header));

	udp = (void *)buffer->packet_start;

	udp->port_src = k_cpu_to_be16(port_src);
	udp->port_dest = k_cpu_to_be16(port_dest);

	length = sizeof(struct k_udp_header) + payload_length;
	udp->length = k_cpu_to_be16(length);

	udp->checksum = 0;
	udp->checksum = k_udp_checksum(udp, K_IPV4(0, 0, 0, 0), ip);

	k_ipv4_build_packet(buffer, length, ip);
}

static k_error_t k_udp_check(struct k_network_buffer *buffer)
{
	k_uint16_t checksum, rx_checksum;
	struct k_udp_header *udp;

	k_network_buffer_adjust_up(buffer, sizeof(struct k_udp_header));

	udp = (void *)buffer->packet_start;

	rx_checksum = udp->checksum;
	udp->checksum = 0;

	checksum = k_udp_checksum(udp, ((struct k_ipv4_info *)buffer->data)->ip_src,
			((struct k_ipv4_info *)buffer->data)->ip_dest);

	udp->checksum = rx_checksum;

	if (rx_checksum != checksum)
		return K_ERROR_BAD_CHECKSUM;

	return K_ERROR_NONE;
}

k_error_t k_udp_rx(struct k_network_buffer *buffer)
{
	k_error_t error;
	struct k_udp_header *udp;

	error = k_udp_check(buffer);
	if (error)
		return error;

	udp = (void *)buffer->start;

	((struct k_ipv4_info *)buffer->data)->port_src = udp->port_src;
	((struct k_ipv4_info *)buffer->data)->port_dest = udp->port_dest;

	switch (udp->port_dest) {
	case K_PORT_DNS:
		break;

	case K_PORT_DHCP_CLIENT:
		if (udp->port_src != K_PORT_DHCP_SERVER)
			return K_ERROR_INVALID_PARAMETER;

		error = k_dhcp_rx(buffer);
		if (error)
			return error;

		break;

	default:
		return K_ERROR_UNSUPPORTED;
	}

	return K_ERROR_NONE;
}

