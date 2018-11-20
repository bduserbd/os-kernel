#ifndef K_NETWORK_BUFFER_H
#define K_NETWORK_BUFFER_H

#include "kernel/include/types.h"

struct k_network_card;

struct k_network_buffer {
	k_uint8_t *start, *end;

	k_uint8_t *packet_start, *packet_end;

	struct k_network_card *card;

	struct k_network_buffer *next, *prev;
};

static inline void k_network_buffer_adjust(struct k_network_buffer *buffer, int length)
{
	k_uint8_t *packet_end;

	packet_end = buffer->packet_start;
	buffer->packet_start -= length;
	buffer->packet_end = packet_end;
}

#endif

