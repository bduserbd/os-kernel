#ifndef K_AHCI_H
#define K_AHCI_H

#include "kernel/include/types.h"

/* AHCI GHC capabilities. */
#define K_AHCI_CAP_NP(cap)	(1 + ((cap) & 0x1f))
#define K_AHCI_CAP_NCS(cap)	(1 + (((cap) >> 8) & 0x1f))

enum {
	K_AHCI_CAP_S64A	= (1 << 31),
};

/* AHCI GHC flags. */
enum {
	K_AHCI_GHC_HR	= (1 << 0),
	K_AHCI_GHC_IE	= (1 << 1),
	K_AHCI_GHC_AE	= (1 << 31),
};

/* Genetal AHCI info. */
struct k_ahci_generic_host_control {
	__u32	cap;
	__u32	ghc;
	__u32	is;
	__u32	pi;
	__u32	vs;
	__u32	ccc_ctl;
	__u32	ccc_ports;
	__u32	em_loc;
	__u32	em_ctl;
	__u32	cap2;
	__u32	bohc;
} __attribute__((packed));

/* AHCI port PxIS. */
enum {
	K_AHCI_PORT_IS_TFES	= (1 << 30),
};

/* AHCI port PxCMD. */
enum {
	K_AHCI_PORT_CMD_ST	= (1 << 0),
	K_AHCI_PORT_CMD_FRE	= (1 << 4),
	K_AHCI_PORT_CMD_FR	= (1 << 14),
	K_AHCI_PORT_CMD_CR	= (1 << 15),
};

/* AHCI port PxTFD. */
enum {
	K_AHCI_PORT_TFD_STS_ERR	= (1 << 0),
	K_AHCI_PORT_TFD_STS_DRQ	= (1 << 3),
	K_AHCI_PORT_TFD_STS_BSY	= (1 << 7),
};

/* AHCI port PxSTSS. */
enum {
	K_AHCI_PORT_SSTS_DET_PRESENT_PHYS	= 0x3,
};

#define K_AHCI_PORT_SSTS_DET(ssts)	(ssts & 0xf)

/* AHCI port registers. */
struct k_ahci_port_registers {
	__u32	clb;
	__u32	clbu;
	__u32	fb;
	__u32	fbu;
	__u32	is;
	__u32	ie;
	__u32	cmd;
	__u32	reserved0;
	__u32	tfd;
	__u32	sig;
	__u32	ssts;
	__u32	sctl;
	__u32	serr;
	__u32	sact;
	__u32	ci;
	__u32	sntf;
	__u32	fbs;
	__u32	devslp;
	__u8	reserved1[0x70 - 0x48];
	__u8	vs[0x80 - 0x70];
} __attribute__((packed));

/* AHCI Command header. */
struct k_ahci_command_header {
	__u16	cfl : 5;
	__u16	a : 1;
	__u16	w : 1;
	__u16	p : 1;
	__u16	r : 1;
	__u16	b : 1;
	__u16	c : 1;
	__u16	reserved0 : 1;
	__u16	pmp : 4;
	__u16	prdtl;
	__u32	prdbc;
	__u32	ctba;
	__u32	ctbau;
	__u32	reserved1[4];
} __attribute__((packed));

/* AHCI Physical Region Descriptor Table. */
#define K_AHCI_PRDT_MAX_DBC	0x400000

struct k_ahci_prdt {
	__u32	dba;
	__u32	dbau;
	__u32	reserved0;
	__u32	dbc : 22;
	__u32	reserved1 : 9;
	__u32	i : 1;
} __attribute__((packed));

/* AHCI Command table. Pointed by the command header. */
#define K_AHCI_PRDT_ENTRIES	8

struct k_ahci_command_table {
	__u8	cfis[0x40];
	__u8	acmd[0x10];
	__u8	reserved0[0x30];
	struct k_ahci_prdt prdt[K_AHCI_PRDT_ENTRIES];
} __attribute__((packed));

/* SATA FIS types. */
enum {
	K_AHCI_FIS_TYPE_REGISTER_HOST_TO_DEVICE	= 0x27,
	K_AHCI_FIS_TYPE_REGISTER_DEVICE_TO_HOST	= 0x34,
	K_AHCI_FIS_TYPE_DMA_SETUP		= 0x41,
	K_AHCI_FIS_TYPE_PIO_SETUP		= 0x5f,
	K_AHCI_FIS_TYPE_SET_DEVICE_BITS		= 0xa1,
};

/* SATA Register - Host to Device FIS. */
enum {
	K_SATA_COMMAND_IDENTIFY			= 0xec,
	K_SATA_COMMAND_READ_SECTORS_DMA_EXT	= 0x25,
};

struct k_ahci_fis_host_to_device {
	__u8	fis_type;
	__u8	pm_port : 4;
	__u8	reserved0 : 3;
	__u8	c : 1;
	__u8	command;
	__u8	features0;
	__u8	lba0;
	__u8	lba1;
	__u8	lba2;
	__u8	device;
	__u8	lba3;
	__u8	lba4;
	__u8	lba5;
	__u8	features1;
	__u8	count0;
	__u8	count1;
	__u8	icc;
	__u8	control;
	__u32	reserved1;
} __attribute__((packed));

/* Register - Device to Host */
struct k_ahci_fis_device_to_host {
	__u8	fis_type;
	__u8	pm_port : 4;
	__u8	reserved0 : 2;
	__u8	i : 1;
	__u8	reserved1 : 1;
	__u8	status;
	__u8	error;
	__u8	lba0;
	__u8	lba1;
	__u8	lba2;
	__u8	device;
	__u8	lba3;
	__u8	lba4;
	__u8	lba5;
	__u8	reserved2;
	__u8	count0;
	__u8	count1;
	__u8	reserved3[6];
} __attribute__((packed));

/* SATA DMA setup - Bidirectional. */
struct k_ahci_fis_dma_setup {
	__u8	fis_type;
	__u8	pm_port	: 4;
	__u8	reserved0 : 1;
	__u8	d : 1;
	__u8	i : 1;
	__u8	a : 1;
	__u16	reserved1;
	__u32	dma_buffer_id_low;
	__u32	dma_buffer_id_high;
	__u32	reserved2;
	__u32	dma_buffer_offset;
	__u32	dma_transfer_count;
	__u32	reserved3;
} __attribute__((packed));

/* SATA PIO setup - Device to Host. */
struct k_ahci_fis_pio_setup {
	__u8	fis_type;
	__u8	pm_port : 4;
	__u8	reserved0 : 1;
	__u8	d : 1;
	__u8	i : 1;
	__u8	reserved1 : 1;
	__u8	status;
	__u8	error;
	__u8	lba0;
	__u8	lba1;
	__u8	lba2;
	__u8	device;
	__u8	lba3;
	__u8	lba4;
	__u8	lba5;
	__u8	reserved2;
	__u8	count0;
	__u8	count1;
	__u8	reserved3;
	__u8	e_status;
	__u16	transfer_count;
	__u16	reserved4;
} __attribute__((packed));

/* SATA Set device bits - Device to Host. */
struct k_ahci_fis_set_device_bits {
	__u8	fis_type;
	__u8	pm_port	: 4;
	__u8	reserved0 : 2;
	__u8	i : 1;
	__u8	n : 1;
	__u8	status_lo : 3;
	__u8	reserved1 : 1;
	__u8	status_hi : 3;
	__u8	reserved2 : 1;
	__u8	error;
	__u32	protocol_specific;
} __attribute__((packed));

/* AHCI Received FIS information. */
struct k_ahci_received_fis {
	struct k_ahci_fis_dma_setup dma_setup;
	__u8	padding0[4];

	struct k_ahci_fis_pio_setup pio_setup;
	__u8	padding1[12];

	struct k_ahci_fis_device_to_host reg;
	__u8	padding2[4];

	struct k_ahci_fis_set_device_bits sdb;

	__u8	ufis[64];

	__u8	reserved0[0x100 - 0xa0];
} __attribute__((packed));

#endif

