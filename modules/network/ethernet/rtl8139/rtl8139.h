#ifndef K_RTL8139_H
#define K_RTL8139_H

/* RTL8139 PCI vendor & device id. */
#define K_RTL8139_PCI_VENDOR_ID	0x10ec
#define K_RTL8139_PCI_DEVICE_ID	0x8139

/* RTL8139 registers. */
enum {
	K_RTL8139_IDR		= 0x00,
	K_RTL8139_RBSTART	= 0x30,
	K_RTL8139_CR		= 0x37,
	K_RTL8139_CAPR		= 0x38,
	K_RTL8139_CBR		= 0x3a,
	K_RTL8139_IMR		= 0x3c,
	K_RTL8139_ISR		= 0x3e,
	K_RTL8139_RCR		= 0x44,
	K_RTL8139_CONFIG1	= 0x52,
};

/* RTL8139 Command Register. */
enum {
	K_RTL8139_CR_TE		= (1 << 2),
	K_RTL8139_CR_RE		= (1 << 3),
	K_RTL8139_CR_RST	= (1 << 4),
};

/* RTL8139 Interrupt Mask Register. */
enum {
	K_RTL8139_IMR_ROK	= (1 << 0),
	K_RTL8139_IMR_TOK	= (1 << 2),
};

/* RTL8139 Interrupt Status Register. */
enum {
	K_RTL8139_ISR_ROK	= (1 << 0),
	K_RTL8139_ISR_TOK	= (1 << 2),
};

/* RTL8139 Receive Configuration Register. */
enum {
	K_RTL8139_RCR_AAP			= (1 << 0),
	K_RTL8139_RCR_APM			= (1 << 1),
	K_RTL8139_RCR_AM			= (1 << 2),
	K_RTL8139_RCR_AB			= (1 << 3),
	K_RTL8139_RCR_WRAP			= (1 << 7),
	K_RTL8139_RCR_RBLEN_8KB_16BYTES		= (0 << 13),
	K_RTL8139_RCR_RBLEN_16KB_16BYTES	= (1 << 13),
	K_RTL8139_RCR_RBLEN_32KB_16BYTES	= (2 << 13),
	K_RTL8139_RCR_RBLEN_64KB_16BYTES	= (3 << 13),
};

#endif

