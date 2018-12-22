#ifndef K_X86_REGISTERS
#define K_X86_REGISTERS

#include "kernel/include/types.h"

struct k_registers {
	// 0x0
	union {
		struct { k_uint64_t rax; };
		struct { k_uint32_t eax; };
		struct { k_uint16_t ax; };
		struct { k_uint8_t al, ah; };
	};

	// 0x8
	union {
		struct { k_uint64_t rbx; };
		struct { k_uint32_t ebx; };
		struct { k_uint16_t bx; };
		struct { k_uint8_t bl, bh; };
	};

	// 0x10
	union {
		struct { k_uint64_t rcx; };
		struct { k_uint32_t ecx; };
		struct { k_uint16_t cx; };
		struct { k_uint8_t cl, ch; };
	};

	// 0x18
	union {
		struct { k_uint64_t rdx; };
		struct { k_uint32_t edx; };
		struct { k_uint16_t dx; };
		struct { k_uint8_t dl, dh; };
	};

	// 0x20
	union {
		struct { k_uint64_t rsp; };
		struct { k_uint32_t esp; };
		struct { k_uint16_t sp; };
	};

	// 0x28
	union {
		struct { k_uint64_t rbp; };
		struct { k_uint32_t ebp; };
		struct { k_uint16_t bp; };
	};

	// 0x28
	union {
		struct { k_uint64_t rdi; };
		struct { k_uint32_t edi; };
		struct { k_uint16_t di; };
	};

	// 0x30
	union {
		struct { k_uint64_t rsi; };
		struct { k_uint32_t esi; };
		struct { k_uint16_t si; };
	};

	// 0x38
	k_uint16_t ds;
	k_uint16_t es;
	k_uint16_t fs;
	k_uint16_t gs;

	// 0x40
	k_uint64_t rflags;

	// 0x48
	union {
		struct { k_uint64_t rip; };
		struct { k_uint32_t eip; };
		struct { k_uint16_t ip; };
	};
} __attribute__((packed));

struct k_int_registers {
	k_uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
	k_uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	k_uint64_t interrupt;
	k_uint64_t error_code, rip, cs, rflags;
} __attribute__((packed));

#endif
