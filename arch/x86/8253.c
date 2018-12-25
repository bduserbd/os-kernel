#include "include/8253.h"
#include "include/speaker.h"
#include "kernel/include/time/device.h"
#include "kernel/include/io/io.h"
#include "kernel/include/video/print.h"

static void k_pit_set_mode(int channel, k_uint16_t ticks)
{
	k_uint8_t command;

	command = K_PIT_COMMAND_PORT_BINARY |
			K_PIT_COMMAND_PORT_MODE_RATE_GENERATOR |
			K_PIT_COMMAND_PORT_ACCESS_MODE_LOW_HIGH_BYTES;

	if (channel == 0)
		command |= K_PIT_COMMAND_PORT_CHANNEL0;
	else if (channel == 2)
		command |= K_PIT_COMMAND_PORT_CHANNEL2;

	k_outb(command, K_PIT_COMMAND_PORT);

	k_outb(ticks & 0xff, K_PIT_CHANNEL0_DATA_PORT);
	k_outb((ticks >> 8) & 0xff, K_PIT_CHANNEL0_DATA_PORT);
}

static k_error_t k_pit_set_periodic_mode(struct k_timer_device *timer)
{
	k_pit_set_mode(0, K_PIT_K_TICKS);

	return K_ERROR_NONE;
}

void k_pit_channel2_sleep(unsigned int microseconds)
{
	k_uint8_t speaker;
	k_uint16_t ticks;

	if (!microseconds)
		return;

	speaker = k_inb(K_PC_SPEAKER_PORT);
	k_outb((speaker & ~K_PC_SPEAKER_SPKR) | K_PC_SPEAKER_GATE2, K_PC_SPEAKER_PORT);

	ticks = microseconds * (K_PIT_FREQUENCY / K_MILLISECONDS_PER_SECOND);
	k_pit_set_mode(2, ticks);

	while (!(k_inb(K_PC_SPEAKER_PORT) & K_PC_SPEAKER_OUT2)) ;

	k_outb(speaker, K_PC_SPEAKER_PORT);
}

static struct k_timer_device k_pit_timer = {
	.type = K_TIMER_DEVICE_PIT,
	.flags = K_TIMER_DEVICE_FLAGS_PERIODIC,
	.irq = 0,
	.frequency = K_PIT_FREQUENCY,
	.set_periodic_mode = k_pit_set_periodic_mode,
};

void k_pit_init(void)
{
	k_timer_device_register(&k_pit_timer);
}

