#include "include/os-interface.h"
#include "include/string.h"

void *memset(void *s, int c, size_t n)
{
	return k_memset(s, c, n);
}

void *memcpy(void *dest, const void *src, size_t n)
{
	return k_memcpy(dest, src, n);
}

int memcmp(const void *m1, const void *m2, size_t n)
{
	return k_memcmp(m1, m2, n);
}

char *strcpy(char *dest, const char *src)
{
	return k_strcpy(dest, src);
}

char *strncpy(char *dest, const char *src, size_t n)
{
	return k_strncpy(dest, src, n);
}

size_t strlen(const char *s)
{
	return k_strlen(s);
}

size_t strnlen(const char *s, k_size_t maxlen)
{
	return k_strnlen(s, maxlen);
}

int strcmp(const char *s1, const char *s2)
{
	return k_strcmp(s1, s2);
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	return k_strncmp(s1, s2, n);
}

char *strcat(char *dest, const char *src)
{
	return k_strcat(dest, src);
}

char *strncat(char *dest, const char *src, size_t n)
{
	return k_strncat(dest, src, n);
}

int isprint(int c)
{
	return k_isprint(c);
}

int isspace(int c)
{
	return k_isspace(c);
}

int isalpha(int c)
{
	return k_isalpha(c);
}

int isdigit(int c)
{
	return k_isdigit(c);
}

int isxdigit(int c)
{
	return k_isxdigit(c);
}

int toupper(int c)
{
	return k_toupper(c);
}

int tolower(int c)
{
	return k_tolower(c);
}

