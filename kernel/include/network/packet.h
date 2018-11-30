#ifndef K_NETWORK_PACKET_H
#define K_NETWORK_PACKET_H

#include "kernel/include/error.h"

struct k_network_buffer;

enum {
	K_NETWORK_PACKET_INVALID,
	K_NETWORK_PACKET_ETHERNET,
};

struct k_network_packet {
	int type;
	k_error_t (*callback)(struct k_network_buffer *);
};

struct k_network_packet *k_network_packet_get(int);

#endif

