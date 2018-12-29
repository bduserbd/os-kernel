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

/* AHCI port PxCMD. */
enum {
	K_AHCI_PORT_CMD_ST	= (1 << 0),
	K_AHCI_PORT_CMD_FRE	= (1 << 4),
	K_AHCI_PORT_CMD_FR	= (1 << 14),
	K_AHCI_PORT_CMD_CR	= (1 << 15),
};

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

#endif

