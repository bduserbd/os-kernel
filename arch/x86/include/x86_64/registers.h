#ifndef K_X86_REGISTERS
#define K_X86_REGISTERS

#include "kernel/include/types.h"

struct k_registers {
	// 0x0
	union {
		struct { k_uint64_t rax; };
		//struct { k_uint32_t eax; };
		//struct { k_uint16_t ax; };
		//struct { k_uint8_t al, ah; };
	};

	// 0x8
	union {
		struct { k_uint64_t rbx; };
		//struct { k_uint32_t ebx; };
		//struct { k_uint16_t bx; };
		//struct { k_uint8_t bl, bh; };
	};

	// 0x10
	union {
		struct { k_uint64_t rcx; };
		//struct { k_uint32_t ecx; };
		//struct { k_uint16_t cx; };
		//struct { k_uint8_t cl, ch; };
	};

	// 0x18
	union {
		struct { k_uint64_t rdx; };
		//struct { k_uint32_t edx; };
		//struct { k_uint16_t dx; };
		//struct { k_uint8_t dl, dh; };
	};

	// 0x20
	union {
		struct { k_uint64_t rsp; };
		//struct { k_uint32_t esp; };
		//struct { k_uint16_t sp; };
	};

	// 0x28
	union {
		struct { k_uint64_t rbp; };
		//struct { k_uint32_t ebp; };
		//struct { k_uint16_t bp; };
	};

	// 0x30
	union {
		struct { k_uint64_t rdi; };
		//struct { k_uint32_t edi; };
		//struct { k_uint16_t di; };
	};

	// 0x38
	union {
		struct { k_uint64_t rsi; };
		//struct { k_uint32_t esi; };
		//struct { k_uint16_t si; };
	};

	// 0x40
	k_uint16_t ds;
	k_uint16_t es;
	k_uint16_t fs;
	k_uint16_t gs;

	// 0x48
	k_uint64_t rflags;

	// 0x50
	union {
		struct { k_uint64_t rip; };
		//struct { k_uint32_t eip; };
		//struct { k_uint16_t ip; };
	};

	// 0x58
	k_uint64_t r8;

	// 0x60
	k_uint64_t r9;

	// 0x68
	k_uint64_t r10;

	// 0x70
	k_uint64_t r11;

	// 0x78
	k_uint64_t r12;

	// 0x80
	k_uint64_t r13;

	// 0x88
	k_uint64_t r14;

	// 0x90
	k_uint64_t r15;
} __attribute__((packed));

struct k_int_registers {
	k_uint64_t rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi;
	k_uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
	k_uint64_t interrupt;
	k_uint64_t error_code, rip, cs, rflags;
} __attribute__((packed));

#endif

