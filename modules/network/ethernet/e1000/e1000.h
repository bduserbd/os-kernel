#ifndef K_E1000_H
#define K_E1000_H

#include "kernel/include/types.h"

/* E1000 registers. */
enum {
	K_E1000_CTRL	= 0x00,
	K_E1000_STATUS	= 0x08,
	K_E1000_EERD	= 0x14,
	K_E1000_ICR	= 0xc0,
	K_E1000_IMS	= 0xd0,
	K_E1000_IMC	= 0xd8,
	K_E1000_RCTL	= 0x100,
	K_E1000_TCTL	= 0x400,
	K_E1000_RDBAL	= 0x2800,
	K_E1000_RDBAH	= 0x2804,
	K_E1000_RDLEN	= 0x2808,
	K_E1000_RDH	= 0x2810,
	K_E1000_RDT	= 0x2818,
	K_E1000_TDBAL	= 0x3800,
	K_E1000_TDBAH	= 0x3804,
	K_E1000_TDLEN	= 0x3808,
	K_E1000_TDH	= 0x3810,
	K_E1000_TDT	= 0x3818,
};

/* E1000 Device Control Register. */
enum {
	K_E1000_CTRL_ASDE	= (1 << 5),
	K_E1000_CTRL_SLU	= (1 << 6),
};

/* E1000 Interrupt Cause Read Register. */
enum {
	K_E1000_ICR_LCS		= (1 << 2),
	K_E1000_ICR_RXSEQ	= (1 << 3),
	K_E1000_ICR_RXDMT0	= (1 << 4),
	K_E1000_ICR_RXO		= (1 << 6),
	K_E1000_ICR_RXT0	= (1 << 7),
	K_E1000_ICR_MDAC	= (1 << 9),
	K_E1000_ICR_PHYINT	= (1 << 12),
};

/* E1000 Interrupt Mask Set/Read Register. */
enum {
	K_E1000_IMS_LSC		= (1 << 2),
	K_E1000_IMS_RXSEQ	= (1 << 3),
	K_E1000_IMS_RXDMT0	= (1 << 4),
	K_E1000_IMS_RXO		= (1 << 6),
	K_E1000_IMS_RXT0	= (1 << 7),
	K_E1000_IMS_MDAC	= (1 << 9),
	K_E1000_IMS_PHYINT	= (1 << 12),
	K_E1000_IMS_GPI		= (3 << 13),
};

/* E1000 Receive Control Register. */
enum {
	K_E1000_RCTL_EN			= (1 << 1),
	K_E1000_RCTL_UPE		= (1 << 3),
	K_E1000_RCTL_MPE		= (1 << 4),
	K_E1000_RCTL_LBM		= (0 << 6),
	K_E1000_RCTL_RDMTS_1_2		= (0 << 8),
	K_E1000_RCTL_RDMTS_1_4		= (1 << 8),
	K_E1000_RCTL_RDMTS_1_8		= (2 << 8),
	K_E1000_RCTL_BAM		= (1 << 15),
	K_E1000_RCTL_BSEX		= (1 << 25),
	K_E1000_RCTL_BSIZE_256		= (3 << 16) | (0 << 25),
	K_E1000_RCTL_BSIZE_512		= (2 << 16) | (0 << 25),
	K_E1000_RCTL_BSIZE_1024		= (1 << 16) | (0 << 25),
	K_E1000_RCTL_BSIZE_2048		= (0 << 16) | (0 << 25),
	K_E1000_RCTL_BSIZE_4096		= (3 << 16) | K_E1000_RCTL_BSEX,
	K_E1000_RCTL_BSIZE_8192		= (2 << 16) | K_E1000_RCTL_BSEX,
	K_E1000_RCTL_BSIZE_16384	= (1 << 16) | K_E1000_RCTL_BSEX,
};

/* E1000 Transmit Control Register. */
enum {
	K_E1000_TCTL_EN			= (1 << 1),
	K_E1000_TCTL_PSP		= (1 << 3),
	K_E1000_TCTL_CT			= (0xf << 4),
	K_E1000_TCTL_COLD_HALF_DUPLEX	= (0x200 << 12),
	K_E1000_TCTL_COLD_FULL_DUPLEX	= (0x40 << 12),
};

/* E1000 Receive Descriptor. */
struct k_e1000_rdesc {
	__u64	buffer;
	__u16	length;
	__u16	checksum;
	__u8	status;
	__u8	errors;
	__u16	special;
} __attribute__((packed));

/* E1000 Receive Descriptor Status. */
enum {
	K_E1000_RDESC_STATUS_DD		= (1 << 0),
	K_E1000_RDESC_STATUS_EOP	= (1 << 1),
};

/* E1000 Transmit Descriptor. */
struct k_e1000_tdesc {
	__u64	buffer;
	__u16	length;
	__u8	cso;
	__u8	cmd;
	__u8	status;
	__u8	css;
	__u16	special;
} __attribute__((packed));

/* E1000 Transmit Descriptor Command. */
enum {
	K_E1000_TDESC_CMD_EOP	= (1 << 0),
	K_E1000_TDESC_CMD_IFCS	= (1 << 1),
	K_E1000_TDESC_CMD_RS	= (1 << 3),
	K_E1000_TDESC_CMD_IDE	= (1 << 7),
};

/* E1000 Transmit Descriptor Status. */
enum {
	K_E1000_TDESC_STATUS_DD	= (1 << 0),
};

/* E1000 TCP/IP Context Descriptor Layout. */
struct k_e1000_context {
	__u8	ipcss;
	__u8	ipcso;
	__u16	ipcse;
	__u8	tucss;
	__u8	tucso;
	__u16	tucse;
	__u32	paylen : 20;
	__u32	dtyp : 4;
	__u32	tucmd : 8;
	__u8	status;
	__u8	hdrlen;
	__u16	mss;
} __attribute__((packed));

/* E1000 EEPROM Read Register. */
enum {
	K_E1000_EERD_START	= (1 << 0),
	K_E1000_EERD_DONE	= (1 << 4),
};

#define K_E1000_EERD_ADDR(addr)	((addr & 0xff) << 8)

#define K_E1000_EERD_DATA(eerd)	((eerd >> 16) & 0xffff)

/* E1000 EEPROM registers. */
enum {
	K_E1000_ETHERNET_ADDRESS0	= 0x0,
	K_E1000_ETHERNET_ADDRESS1	= 0x1,
	K_E1000_ETHERNET_ADDRESS2	= 0x2,
	K_E1000_EEPROM_INIT_CONTROL1	= 0xa,
};

/* E1000 EEPROM Initialization Control Word 1. */
#define K_E1000_EEPROM_INIT_CONTROL1_SIGNATURE_VALID(icw1)	\
	(((icw1 >> 14) & 0x3) == 0b01)

#endif

