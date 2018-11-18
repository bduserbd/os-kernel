#ifndef K_X86_REGISTERS
#define K_X86_REGISTERS

#include "kernel/include/types.h"

struct k_registers {
	union {
		struct { k_uint32_t eax; };
		struct { k_uint16_t ax; };
		struct { k_uint8_t al, ah; };
	};

	union {
		struct { k_uint32_t ebx; };
		struct { k_uint16_t bx; };
		struct { k_uint8_t bl, bh; };
	};

	union {
		struct { k_uint32_t ecx; };
		struct { k_uint16_t cx; };
		struct { k_uint8_t cl, ch; };
	};

	union {
		struct { k_uint32_t edx; };
		struct { k_uint16_t dx; };
		struct { k_uint8_t dl, dh; };
	};

	union {
		struct { k_uint32_t esp; };
		struct { k_uint16_t sp; };
	};

	union {
		struct { k_uint32_t ebp; };
		struct { k_uint16_t bp; };
	};

	union {
		struct { k_uint32_t edi; };
		struct { k_uint16_t di; };
	};

	union {
		struct { k_uint32_t esi; };
		struct { k_uint16_t si; };
	};

	k_uint16_t ds;
	k_uint16_t es;
	k_uint16_t fs;
	k_uint16_t gs;

	k_uint32_t eflags;

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

#endif

