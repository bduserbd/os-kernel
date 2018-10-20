#ifndef K_8259A_H
#define K_8259A_H

#include "kernel/include/types.h"

#define K_PIC_MASTER_COMMAND	0x20
#define K_PIC_MASTER_DATA	0x21

#define K_PIC_SLAVE_COMMAND	0xa0
#define K_PIC_SLAVE_DATA	0xa1

#define K_PIC_ICW1_ICW4		(1 << 0)
#define K_PIC_ICW1_SNGL		(1 << 1)
#define K_PIC_ICW1_ADI		(1 << 2)
#define K_PIC_ICW1_LTIM		(1 << 3)
#define K_PIC_ICW1		(1 << 4)

#define K_PIC_ICW3(ir)		(1 << ir)

#define K_PIC_ICW4_MICRO_PM	(1 << 0)
#define K_PIC_ICW4_AEOI		(1 << 1)
#define K_PIC_ICW4_MS		(1 << 2)
#define K_PIC_ICW4_BUF		(1 << 3)
#define K_PIC_ICW4_SFNM		(1 << 4)

void k_pic_init(void);

#endif

