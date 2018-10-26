#include "include/8259a.h"
#include "include/idt.h"
#include "kernel/include/io/io.h"
#include "kernel/include/video/print.h"

void k_pic_outb(k_uint8_t v, k_uint8_t port)
{
	k_outb(v, port);
	k_io_wait();
}

void k_pic_disable(void)
{
	k_outb(0xff, K_PIC_MASTER_DATA);
	k_outb(0xff, K_PIC_SLAVE_DATA);
}

void k_pic_init(void)
{
	k_pic_disable();

	k_pic_outb(K_PIC_ICW1 | K_PIC_ICW1_ICW4, K_PIC_MASTER_COMMAND);
	k_pic_outb(K_IRQ_MASTER_START, K_PIC_MASTER_DATA);
	k_pic_outb(K_PIC_ICW3(2), K_PIC_MASTER_DATA);
	k_pic_outb(K_PIC_ICW4_MICRO_PM, K_PIC_MASTER_DATA);
	k_pic_outb(0x0, K_PIC_MASTER_DATA);

	k_pic_outb(K_PIC_ICW1 | K_PIC_ICW1_ICW4, K_PIC_SLAVE_COMMAND);
	k_pic_outb(K_IRQ_SLAVE_START, K_PIC_SLAVE_DATA);
	k_pic_outb(2, K_PIC_SLAVE_DATA);
	k_pic_outb(K_PIC_ICW4_MICRO_PM, K_PIC_SLAVE_DATA);
	k_pic_outb(0x0, K_PIC_SLAVE_DATA);
}

