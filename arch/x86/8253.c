#include "include/8253.h"
#include "kernel/include/io/io.h"

void k_pit_init(void)
{
	k_uint16_t divisor = (k_uint16_t)(0xff);

	k_outb(0x36, 0x43);

	k_outb(divisor & 0xff, 0x40);
	k_outb((divisor >> 8) & 0xff, 0x40);
}

