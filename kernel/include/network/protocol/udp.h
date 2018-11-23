#ifndef K_UDP_H
#define K_UDP_H

#include "kernel/include/network/network.h"
#include "kernel/include/network/protocol/ipv4.h"
#include "kernel/include/network/protocol/tcp-udp-ports.h"

struct k_udp_header {
	__u16	port_src;
	__u16	port_dest;
	__u16	length;
	__u16	checksum;
} __attribute__((packed));

void k_udp_build_packet(struct k_network_buffer *, k_uint16_t, k_port_t, k_port_t,
		k_ipv4_t);
k_error_t k_udp_rx(struct k_network_buffer *);

#endif

