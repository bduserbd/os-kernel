#ifndef K_NETWORK_BUFFER
#define K_NETWORK_BUFFER

#include "kernel/include/types.h"

struct k_network_buffer {
	k_uint8_t *start, *end;

	struct k_network_buffer *next, *prev;
};

#endif

