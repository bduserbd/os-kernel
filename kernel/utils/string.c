#include "include/string.h"

void *k_memset(void *s, int c, k_size_t n)
{
	char *d;

	if (!s)
		return NULL;

	d = (char *)s;

	while (--n)
		*d++ = c;		

	return s;
}

void *k_memcpy(void *dest, const void *src, k_size_t n)
{
	char *d;
	const char *s;

	if (!dest || !src)
		return NULL;

	d = (char *)dest;
	s = (const char *)src;

	if (s > d)
		while (n--)
			*d++ = *s++;
	else {
		d += n;
		s += n;

		while (n--)
			*--d = *--s;
	}

	return dest;
}

int k_memcmp(const void *m1, const void *m2, k_size_t n)
{
	k_size_t i;
	const k_uint8_t *_m1 = m1;
	const k_uint8_t *_m2 = m2;

	for (i = 0; i < n; i++, _m1++, _m2++)
		if (*_m1 < *_m2)
			return -1;
		else if (*_m1 > *_m2)
			return 1;

	return 0;
}

