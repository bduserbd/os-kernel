#ifndef K_IPV4_INFO_H
#define K_IPV4_INFO_H

#include "kernel/include/network/protocol/ethernet.h"
#include "kernel/include/network/protocol/ipv4.h"
#include "kernel/include/network/protocol/tcp-udp-ports.h"

struct k_ipv4_info {
	k_uint8_t mac_src[K_MAC_LENGTH];
	k_uint8_t mac_dest[K_MAC_LENGTH];

	k_ipv4_t ip_src;
	k_ipv4_t ip_dest;

	k_port_t port_src;
	k_port_t port_dest;
};

#endif

