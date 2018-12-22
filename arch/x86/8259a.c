#include "include/8259a.h"
#include "include/idt.h"
#include "kernel/include/io/io.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

static void k_pic_outb(k_uint8_t v, k_uint8_t port)
{
	k_outb(v, port);
	k_io_wait();
}

static void k_pic_ack(struct k_irq_chip *chip, unsigned int irq)
{
	if (irq & 0x8)
		k_pic_outb(K_PIC_EOI, K_PIC_SLAVE_COMMAND);

	k_pic_outb(K_PIC_EOI, K_PIC_MASTER_COMMAND);
}

static void k_pic_unmask(struct k_irq_chip *chip, unsigned int irq)
{
	k_uint8_t port;
	k_uint8_t mask;

	if (irq > 0xf)
		return;

	if (irq & 0x8) {
		irq -= 8;
		port = K_PIC_SLAVE_DATA;
	} else
		port = K_PIC_MASTER_DATA;

	mask = k_inb(port);
	mask &= ~(1 << irq);
	k_pic_outb(mask, port);
}

static void k_pic_mask(struct k_irq_chip *chip, unsigned int irq)
{
	k_uint8_t port;
	k_uint8_t mask;

	if (irq > 0xf)
		return;

	if (irq & 0x8) {
		irq -= 8;
		port = K_PIC_SLAVE_DATA;
	} else
		port = K_PIC_MASTER_DATA;

	mask = k_inb(port);
	mask |= (1 << irq);
	k_pic_outb(1 << irq, port);
}

static void k_pic_reset(struct k_irq_chip *chip)
{
	k_outb(0xff, K_PIC_MASTER_DATA);
	k_outb(0xff, K_PIC_SLAVE_DATA);
}

static k_error_t k_pic_start(struct k_irq_chip *chip)
{
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

	return K_ERROR_NONE;
}

static unsigned int k_pic_int_to_irq(struct k_irq_chip *chip, unsigned int interrupt)
{
	if ((interrupt >= K_IRQ_SLAVE_START) && (interrupt < K_IRQ_SLAVE_START + 8))
		return 8 + interrupt - K_IRQ_SLAVE_START;
	else if ((interrupt >= K_IRQ_MASTER_START) && (interrupt < K_IRQ_MASTER_START + 8))
		return interrupt - K_IRQ_MASTER_START;
	else
		return K_INVALID_IRQ;
}

static struct k_irq_chip k_8259a_irq_chip = {
	.name = "8259A",
	.start = k_pic_start,
	.reset = k_pic_reset,
	.ack = k_pic_ack,
	.mask = k_pic_mask,
	.unmask = k_pic_unmask,
	.int_to_irq = k_pic_int_to_irq,
	.data = NULL,
};

void k_pic_init(void)
{
	//k_irq_register_chip(&k_8259a_irq_chip);
	k_8259a_irq_chip.reset(&k_8259a_irq_chip);
}

