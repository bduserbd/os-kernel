#include "include/video/print.h"
#include "include/string.h"
#include "include/div64.h"
#include "include/modules/export-symbol.h"

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

void k_putchar(char c)
{
	char s[2];

	s[0] = c;
	s[1] = 0;

	k_puts(s);
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
		k_puts(s);						\
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
                        s[0] = (big ? 'A' : 'a') + digit - 0xA;		\
									\
                k_puts(s);						\
        }

void k_puthex(k_uint32_t number, int big)
{
	K_PRINT_HEX
}

void k_puthex64(k_uint64_t number, int big)
{
	K_PRINT_HEX
}

void k_putn(k_uint32_t number, int decimal)
{
	int i;
	char s[2];
	k_int8_t reverse[10] = { 0 };

	if (!number)
		k_putchar('0');

	if (decimal)
		if (number & (1 << 31)) {
			number = 1 + (number ^ 0xffffffff);
			k_putchar('-');
		}

	for (i = 0; number; i++) {
		reverse[i] = number % 10;
		number /= 10;
	}

	s[1] = 0;
	for (i--; i >= 0; i--) {
		s[0] = '0' + reverse[i];
		k_puts(s);
	}
}

void k_putn64(k_uint64_t number, int decimal)
{
	int i;
	char s[2];
	k_uint64_t q, r;
	k_uint64_t reverse[20] = { 0 };

	if (!number)
		k_putchar('0');

	if (decimal)
		if (number & (1ULL << 63)) {
			number = 1 + (number ^ 0xffffffffffffffffULL);
			k_putchar('-');
		}

	for (i = 0; number; i++) {
		k_div64(number, 10, &q, &r);

		reverse[i] = r;
		number = q;
	}

	s[1] = 0;
	for (i--; i >= 0; i--) {
		s[0] = '0' + reverse[i];
		k_puts(s);
	}
}

#define  K_PRINTF_FORMAT_DECIMAL	(1 << 0)
#define  K_PRINTF_FORMAT_UNSIGNED	(1 << 1)
#define  K_PRINTF_FORMAT_OCTAL		(1 << 2)
#define  K_PRINTF_FORMAT_HEX_SMALL	(1 << 3)
#define  K_PRINTF_FORMAT_HEX_BIG	(1 << 4)
#define  K_PRINTF_FORMAT_CHAR		(1 << 5)
#define  K_PRINTF_FORMAT_STRING		(1 << 6)
#define  K_PRINTF_FORMAT_POINTER	(1 << 7)

int k_vprintf(const char *fmt, va_list ap)
{
	int n;
	int width, precision;
	int length_modifier;
	unsigned int flags;

	n = 0;

	while (*fmt) {
		if (*fmt != '%') {
			n++;
			k_putchar(*fmt);

			fmt++;

			continue;
		}

		fmt++;
		if (*fmt == '%') {
			n++;
			k_putchar(*fmt);

			fmt++;

			continue;
		}

		flags = 0;
		length_modifier = 0;

#if 0
		if (c == '#') {

		} else if (c == '0') {
			fmt++;
		}
#endif

		/* Width. */
		for (width = 0; k_isdigit(*fmt); fmt++, width++) ;

		/* Precision. */
		if (*fmt == '.') {
			fmt++;
			for (precision = 0; k_isdigit(*fmt); fmt++, precision++) ;
		}

		/* Length. */
		switch (*fmt) {
		case 'h':
			fmt++;

			if (*fmt == 'h') {
				length_modifier = sizeof(k_uint8_t);
				fmt++;
			} else
				length_modifier = sizeof(k_uint16_t);

			break;

		case 'l':
			fmt++;

			if (*fmt == 'l') {
				length_modifier = sizeof(unsigned long long int);
				fmt++;
			} else
				length_modifier = sizeof(unsigned long int);

			break;

		default:
			break;
		}

		/* Type. */
		switch (*fmt) {
		case 'd':
		case 'i':
			flags |= K_PRINTF_FORMAT_DECIMAL;

			if (!length_modifier)
				length_modifier = sizeof(int);

			break;

		case 'u':
			flags |= K_PRINTF_FORMAT_UNSIGNED;

			if (!length_modifier)
				length_modifier = sizeof(unsigned int);

			break;

		case 'o':
			flags |= K_PRINTF_FORMAT_OCTAL;

			if (!length_modifier)
				length_modifier = sizeof(unsigned int);

			break;

		case 'x':
			flags |= K_PRINTF_FORMAT_HEX_SMALL;

			if (!length_modifier)
				length_modifier = sizeof(unsigned int);

			break;

		case 'X':
			flags |= K_PRINTF_FORMAT_HEX_BIG;

			if (!length_modifier)
				length_modifier = sizeof(unsigned int);

			break;

		case 'c':
			flags |= K_PRINTF_FORMAT_CHAR;

			if (!length_modifier)
				length_modifier = sizeof(char);

			break;

		case 's':
			flags |= K_PRINTF_FORMAT_STRING;
			break;

		case 'p':
			flags |= K_PRINTF_FORMAT_POINTER;
			break;

		default:
			break;
		}

		if (length_modifier > 0 && flags == 0)
			return -1;

		if (flags & K_PRINTF_FORMAT_STRING)
			k_puts((const char *)va_arg(ap, unsigned long));
		else {
			k_uint64_t v = 0;

			switch(length_modifier) {
			case sizeof(k_uint8_t):
				v = (k_uint8_t)va_arg(ap, k_uint32_t);
				break;

			case sizeof(k_uint16_t):
				v = (k_uint16_t)va_arg(ap, k_uint32_t);
				break;

			case sizeof(k_uint32_t):
				v = va_arg(ap, k_uint32_t);
				break;

			case sizeof(k_uint64_t):
				v = va_arg(ap, k_uint64_t);
				break;

			default:
				break;
			}

			if (flags & K_PRINTF_FORMAT_DECIMAL)
				k_putn64(v, 1);
			else if (flags & K_PRINTF_FORMAT_UNSIGNED)
				k_putn64(v, 0);
			else if (flags & K_PRINTF_FORMAT_OCTAL) {

			} else if (flags & K_PRINTF_FORMAT_HEX_SMALL)
				k_puthex64(v, 0);
			else if (flags & K_PRINTF_FORMAT_HEX_BIG)
				k_puthex64(v, 1);
			else if (flags & K_PRINTF_FORMAT_CHAR)
				k_putchar((char)v);
			else if (flags & K_PRINTF_FORMAT_POINTER) {

			}
		}

		fmt++;
	}

	return n;
}

int k_printf(const char *fmt, ...)
{
	int n;
	va_list ap;

	va_start(ap, fmt);
	n = k_vprintf(fmt, ap);

	va_end(ap);

	return n;
}
K_EXPORT_FUNC(k_printf);

