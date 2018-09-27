#include "include/types.h"

static void (*k_print_callback)(const char *) = NULL;

void k_print_set_output_callback(void (*callback)(const char *))
{
	k_print_callback = callback;
}

void k_puts(const char *str)
{
	if (k_print_callback)
		k_print_callback(str);
}

#define K_PRINT_HEX							\
        char s[2];							\
        int zero = 0, bits;						\
        k_uint8_t digit;						\
									\
	s[0] = '0';							\
	s[1] = '\0';							\
									\
        if (number == 0) {						\
		k_puts(s);					\
                return;							\
        }								\
									\
        for (bits = sizeof(number) * 8 - 4; bits >= 0; bits -= 4) {	\
                digit = (number >> bits) & 0xf;				\
									\
                if (digit == 0) {					\
                        if (!zero)					\
                                continue;				\
                } else							\
                        zero = 1;					\
									\
                if (digit < 0xA)					\
                        s[0] = '0' + digit;				\
                else							\
                        s[0] = 'a' + digit - 0xA;			\
									\
                k_puts(s);					\
        }

void k_puthex(k_uint32_t number)
{
	K_PRINT_HEX
}

void k_puthhex64(k_uint64_t number)
{
	K_PRINT_HEX
}

