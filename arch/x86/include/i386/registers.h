#ifndef K_X86_REGISTERS
#define K_X86_REGISTERS

#include "kernel/include/types.h"

struct k_registers {
	// 0x0
	union {
		struct { k_uint32_t eax; };
		struct { k_uint16_t ax; };
		struct { k_uint8_t al, ah; };
	};

	// 0x4
	union {
		struct { k_uint32_t ebx; };
		struct { k_uint16_t bx; };
		struct { k_uint8_t bl, bh; };
	};

	// 0x8
	union {
		struct { k_uint32_t ecx; };
		struct { k_uint16_t cx; };
		struct { k_uint8_t cl, ch; };
	};

	// 0xc
	union {
		struct { k_uint32_t edx; };
		struct { k_uint16_t dx; };
		struct { k_uint8_t dl, dh; };
	};

	// 0x10
	union {
		struct { k_uint32_t esp; };
		struct { k_uint16_t sp; };
	};

	// 0x14
	union {
		struct { k_uint32_t ebp; };
		struct { k_uint16_t bp; };
	};

	// 0x18
	union {
		struct { k_uint32_t edi; };
		struct { k_uint16_t di; };
	};

	// 0x1c
	union {
		struct { k_uint32_t esi; };
		struct { k_uint16_t si; };
	};

	// 0x20
	k_uint16_t ds;
	k_uint16_t es;
	k_uint16_t fs;
	k_uint16_t gs;

	// 0x28
	k_uint32_t eflags;

	// 0x2c
	union {
		struct { k_uint32_t eip; };
		struct { k_uint16_t ip; };
	};
} __attribute__((packed));

struct k_int_registers {
	k_uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	k_uint32_t interrupt;
	k_uint32_t error_code, eip, cs, eflags;
} __attribute__((packed));

struct k_system_call_registers {
	k_uint32_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax;
	k_uint32_t eip, cs, eflags, user_esp, ss;
} __attribute__((packed));

#endif

