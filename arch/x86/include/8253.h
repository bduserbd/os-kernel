#ifndef K_PIT_H
#define K_PIT_H

#include "kernel/include/types.h"

/* PIT frequency. */
#define K_PIT_FREQUENCY	1193182
#define K_PIT_K_TICKS	(K_PIT_FREQUENCY / K_HZ)

/* I/O ports of PIT channels. */
#define K_PIT_CHANNEL0_DATA_PORT	0x40
#define K_PIT_CHANNEL1_DATA_PORT	0x41
#define K_PIT_CHANNEL2_DATA_PORT	0x42

#define K_PIT_COMMAND_PORT		0x43

/* PIT command register. */
#define K_PIT_COMMAND_PORT_CHANNEL0	(0 << 6)
#define K_PIT_COMMAND_PORT_CHANNEL1	(1 << 6)
#define K_PIT_COMMAND_PORT_CHANNEL2	(2 << 6)
#define K_PIT_COMMAND_PORT_READ_BACK	(3 << 6)

#define K_PIT_COMMAND_PORT_ACCESS_MODE_LATCH_COUNT	(0 << 4)
#define K_PIT_COMMAND_PORT_ACCESS_MODE_LOW_BYTE_ONLY	(1 << 4)
#define K_PIT_COMMAND_PORT_ACCESS_MODE_HIGH_BYTE_ONLY	(2 << 4)
#define K_PIT_COMMAND_PORT_ACCESS_MODE_LOW_HIGH_BYTES	(3 << 4)

#define K_PIT_COMMAND_PORT_MODE_RATE_GENERATOR		(2 << 1)
#define K_PIT_COMMAND_PORT_MODE_SQUARE_WAVE_GENERATOR	(3 << 1)

#define K_PIT_COMMAND_PORT_BINARY	(0 << 0)
#define K_PIT_COMMAND_PORT_BCD		(1 << 0)

void k_pit_init(void);
void k_pit_channel2_sleep(unsigned int);

#endif

