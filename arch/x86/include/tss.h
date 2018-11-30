#ifndef K_TSS_H
#define K_TSS_H

#include "kernel/include/types.h"

struct k_tss {
	__u16	previous_task_link;
	__u16	reserved0;

	__u32	esp0;
	__u16	ss0;
	__u16	reserved1;

	__u32	esp1;
	__u16	ss1;
	__u16	reserved2;

	__u32	esp2;
	__u16	ss2;
	__u16	reserved3;

	__u32	cr3;
	__u32	eip;
	__u32	eflags;
	__u32	eax;
	__u32	ecx;
	__u32	edx;
	__u32	ebx;
	__u32	esp;
	__u32	ebp;
	__u32	esi;
	__u32	edi;

	__u16	es;
	__u16	reserved4;

	__u16	cs;
	__u16	reserved5;

	__u16	ss;
	__u16	reserved6;

	__u16	ds;
	__u16	reserved7;

	__u16	fs;
	__u16	reserved8;

	__u16	gs;
	__u16	reserved9;

	__u16	ldt_segment_selector;
	__u32	reserved10;

	__u16	io_map_base_address;
} __attribute__((packed));

#endif

