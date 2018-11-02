#ifndef K_RTL8139_H
#define K_RTL8139_H

/* RTL8139 PCI vendor & device id. */
#define K_RTL8139_PCI_VENDOR_ID	0x10ec
#define K_RTL8139_PCI_DEVICE_ID	0x8139

/* RTL8139 registers. */
#define K_RTL8139_IDR		0x00

#define K_RTL8139_RBSTART	0x30

#define K_RTL8139_CR		0x37
#define K_RTL8139_CR_RST	(1 << 4)

#define K_RTL8139_CONFIG1	0x52

#endif

