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

