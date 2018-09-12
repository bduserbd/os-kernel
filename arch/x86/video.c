#include "include/types.h"

static unsigned char *vid = (unsigned char *)0xb8000;

void puts(const char *str)
{
        static int x = 0;

        while (*str) {
                vid[x] = *str;
                str++;
                x+=2;
        }
}

void puthex(k_uint32_t number)
{
        char alpha[2] = "0";
        int was_not_zero = 0, bits;
        k_uint8_t digit;

        if (number == 0) {
                puts(alpha);
        	puts(" ");
                return;
        }

        for (bits = sizeof(number) * 8 - 4; bits >= 0; bits -= 4) {
                digit = (number >> bits) & 0xf;

                if (digit == 0) {
                        if (!was_not_zero)
                                continue;
                } else
                        was_not_zero = 1;

                if (digit < 0xA)
                        alpha[0] = '0' + digit;
                else
                        alpha[0] = 'a' + (digit - 0xA);

                puts(alpha);
        }
        puts(" ");
}

