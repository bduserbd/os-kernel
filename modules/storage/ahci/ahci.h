#ifndef K_AHCI_H
#define K_AHCI_H

#include "kernel/include/types.h"

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
};

/* AHCI GHC flags. */
enum {
	K_AHCI_GHC_HR	= (1 << 0),
	K_AHCI_GHC_AE	= (1 << 31),
};


#endif

