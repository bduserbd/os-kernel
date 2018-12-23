#include "include/8253.h"
#include "kernel/include/irq/irq.h"
#include "kernel/include/io/io.h"
#include "kernel/include/task/task.h"
#include "kernel/include/video/print.h"

static k_error_t k_pit_irq_handler(unsigned int irq, void *context)
{
	static int ticks = 0;

	ticks++;
	if (ticks == 5) {
		k_printf("$");
		ticks = 0;
	}

	//k_task_switch(context);

	return K_ERROR_NONE;
}

void k_pit_init(void)
{
	k_error_t error;
	const k_uint16_t divisor = 0xfff;

	error = k_irq_request(0, k_pit_irq_handler, 0, NULL);
	if (error)
		return;

	k_outb(K_PIT_COMMAND_PORT_BINARY |
			K_PIT_COMMAND_PORT_MODE_SQUARE_WAVE_GENERATOR |
			K_PIT_COMMAND_PORT_ACCESS_MODE_LOW_HIGH_BYTES |
			K_PIT_COMMAND_PORT_CHANNEL0,
			K_PIT_COMMAND_PORT);

	k_outb(divisor & 0xff, K_PIT_CHANNEL0_DATA_PORT);
	k_outb((divisor >> 8) & 0xff, K_PIT_CHANNEL0_DATA_PORT);
}

