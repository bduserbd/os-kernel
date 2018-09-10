#include "include/idt.h"

void puts(const char *);
void puthex(k_uint64_t);

struct k_idt_interrupt_gate k_idt[256] __attribute__((aligned(0x8)));

void foo(void)
{
	int i;
	int *e, error_code_interrupts[7] = { 0x8, 0xa, 0xb, 0xc, 0xd, 0x11, 0x1e };

	puts("Wow");

	e = &error_code_interrupts[0];
	for (i = 0; i < 32; i++) {
		if (i == *e) {
			e++;
		} else {
			if (i == 0xe) {

			} else {


			}
		}
	}
}

