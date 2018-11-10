#ifndef K_ETHERNET_H
#define K_ETHERNET_H

#include "kernel/include/types.h"
#include "kernel/include/network/buffer.h"

#define K_MAC_LENGTH	6

#define K_ETHERNET_PROTOCOL_IP	0x800
#define K_ETHERNET_PROTOCOL_ARP	0x806

struct k_ethernet_header {
	__u8	mac_dest[K_MAC_LENGTH];
	__u8	mac_src[K_MAC_LENGTH];
	__u16	protocol;
} __attribute__((packed));

void k_ethernet_build_packet(struct k_network_buffer *, k_uint16_t,
		k_uint8_t [K_MAC_LENGTH], k_uint8_t [K_MAC_LENGTH]);

void k_ethernet_build_broadcast_packet(struct k_network_buffer *, k_uint16_t,
		k_uint8_t [K_MAC_LENGTH]);

#endif

