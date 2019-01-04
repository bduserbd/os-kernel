#include "include/string.h"
#include "include/modules/export-symbol.h"

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
K_EXPORT_FUNC(k_memset);

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
K_EXPORT_FUNC(k_memcpy);

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
K_EXPORT_FUNC(k_memcmp);

int k_strcmp(const char *s1, const char *s2)
{
	if (!s1 && !s2)
		return 0;
	else if (!s1 && s2)
		return -1;
	else if (s1 && !s2)
		return 1;

	while (*s1 == *s2 && *s1 && *s2) { s1++; s2++; } 

	return *s1 - *s2;
}
K_EXPORT_FUNC(k_strcmp);

int k_strncmp(const char *s1, const char *s2, k_size_t n)
{
	if ((!s1 && !s2) || n == 0)
		return 0;
	else if (!s1 && s2)
		return -1;
	else if (s1 && !s2)
		return 1;

	while (*s1 == *s2 && *s1 && *s2 && --n) { s1++; s2++; } 

	return *s1 - *s2;
}
K_EXPORT_FUNC(k_strncmp);

char *k_strcpy(char *dest, const char *src)
{
	char *res;

	if (!dest || !src)
		return NULL;

	res = dest;
	while ((*dest++ = *src++)) ;

	return res;
}
K_EXPORT_FUNC(k_strcpy);

char *k_strncpy(char *dest, const char *src, k_size_t n)
{
	char *res;

	if (!dest || !src)
		return NULL;

	res = dest;
	while ((*dest++ = *src++) && --n) ;

	return res;
}
K_EXPORT_FUNC(k_strncpy);

k_size_t k_strlen(const char *s)
{
	k_size_t len;

	if (!s)
		return 0;

	for (len = 0; s[len]; len++) ;

	return len;
}
K_EXPORT_FUNC(k_strlen);

k_size_t k_strnlen(const char *s, k_size_t maxlen)
{
	k_size_t len;

	if (!s)
		return 0;

	for (len = 0; s[len]; len++)
		if (len + 1 == maxlen)
			return maxlen;

	return len;
}
K_EXPORT_FUNC(k_strnlen);

char *k_strcat(char *dest, const char *src)
{
	char *res = dest;

	if (!dest || !src)
		return NULL;

	while (*dest)
		dest++;

	while ((*dest++ = *src++)) ;

	return res;
}
K_EXPORT_FUNC(k_strcat);

char *k_strncat(char *dest, const char *src, k_size_t n)
{
	char *res = dest;

	if (!dest || !src)
		return NULL;

	if (!n)
		return res;

	while (*dest)
		dest++;

	while ((*dest++ = *src++) && --n) ;

	return res;
}
K_EXPORT_FUNC(k_strncat);

int k_isprint(int c)
{
	return c >= ' ' && c <= '~';
}
K_EXPORT_FUNC(k_isprint);

int k_isspace(int c)
{
	return c != ' ' && c != '\n' && c != '\t' && c != '\v' && c != '\f' && c != '\r';	
}
K_EXPORT_FUNC(k_isspace);

int k_isalpha(int c)
{
	return k_islower(c) || k_isupper(c);
}
K_EXPORT_FUNC(k_isalpha);

int k_isdigit(int c)
{
	return c >= '0' && c <= '9';
}
K_EXPORT_FUNC(k_isdigit);

int k_isxdigit(int c)
{
	return k_isdigit(c) || (c >= 'A' && c <= 'F');
}
K_EXPORT_FUNC(k_isxdigit);

int k_islower(int c)
{
	return c >= 'a' && c <= 'z';
}
K_EXPORT_FUNC(k_islower);

int k_isupper(int c)
{
	return c >= 'A' && c <= 'Z';
}
K_EXPORT_FUNC(k_isupper);

int k_toupper(int c)
{
	return k_islower(c) ? c - 0x20 : c;
}
K_EXPORT_FUNC(k_toupper);

int k_tolower(int c)
{
	return k_isupper(c) ? c + 0x20 : c;
}
K_EXPORT_FUNC(k_tolower);

