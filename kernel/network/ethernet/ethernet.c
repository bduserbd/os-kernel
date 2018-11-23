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

static k_uint32_t k_ethernet_fcs(k_uint8_t buf[], int length)
{
	int i, j;
	unsigned long fcs;

	fcs = 0xffffffffUL;

	for (i = 0; i < length; i++) {
		fcs ^= buf[i];
		for (j = 0; j < 8; j++)
			if (fcs & 1)
				fcs = (fcs >> 1) ^ 0xedb88320UL;
			else
				fcs >>= 1;
	}

	return fcs ^ 0xffffffffUL;
}

static k_error_t k_ethernet_check(struct k_network_buffer *buffer)
{
	k_uint32_t rx_fcs;

	rx_fcs = *(k_uint32_t *)(buffer->end - sizeof(k_uint32_t));

	if (rx_fcs != k_ethernet_fcs(buffer->start, buffer->end - buffer->start -
			sizeof(k_uint32_t)))
		return K_ERROR_BAD_CHECKSUM;

	buffer->packet_start = buffer->packet_end = buffer->start;
	k_network_buffer_adjust_up(buffer, sizeof(struct k_ethernet_header));

	return K_ERROR_NONE;
}

k_error_t k_ethernet_rx(struct k_network_buffer *buffer)
{
	k_error_t error;
	struct k_ethernet_header *ethernet;

	error = k_ethernet_check(buffer);
	if (error)
		return error;

	ethernet = (void *)buffer->packet_start;

	switch (k_be16_to_cpu(ethernet->protocol)) {
	case K_ETHERNET_PROTOCOL_IP:
		error = k_ipv4_rx(buffer);
		if (error)
			return error;

		break;

	case K_ETHERNET_PROTOCOL_ARP:
		break;

	default:
		return K_ERROR_UNSUPPORTED;
	}

	return K_ERROR_NONE;
}

