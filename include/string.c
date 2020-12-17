
/*
 * String functions, some of them are public domain.
 */

#include <stdint.h>




s8 strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s1 == *s2)
		s1++, s2++;

	if (*s1 == *s2)
		return 0;

	return (*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : 1;
}



s8 strncmp(const char *s1, const char *s2, size_t l)
{
	while (--l > 0 && *s1 == *s2)
		s1++, s2++;

	if (l <= 0 || *s1 == '\0')
		return 0;

	return (*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : 1;
}



void strcpy(char *dst, char *src)
{
	while (*src)
		*dst++ = *src++;
}



void strncpy(char *dst, char *src, s64 l)
{
	while (*src) {
		if (--l < 0)
			break;
		*dst++ = *src++;
	}
}



void memcpy(void *dst, void *src, size_t l)
{
	/* Could be made faster... */
	while (l--)
		*(u8 *)dst++ = *(u8 *)src++;
}



size_t strlen(const char *s)
{
	u16 i = 0;

	while (*s++)
		++i;

	return i;
}



char *strchr(const char *s, int c)
{
	while (*s != '\0' && *s != (char)c)
		s++;

	return (*s == c) ? (char *)s : NULL;
}



size_t strcspn(const char *s1, const char *s2)
{
	const char *sc1;

	for (sc1 = s1; *sc1 != '\0'; sc1++)
		if (strchr(s2, *sc1) != NULL)
			return sc1 - s1;

	return sc1 - s1;
}



size_t strspn(const char *s1, const char *s2)
{
	const char *sc1;

	for (sc1 = s1; *sc1 != '\0'; sc1++)
		if (strchr(s2, *sc1) == NULL)
			return sc1 - s1;

	return sc1 - s1;
}



void *memset(void *s, u8 c, size_t n)
{
	u8 *p = s;

	while (n--)
		*p++ = c;

	return s;
}



s16 strlen2sep(const char *s, const char sep)
{
	u8 i = 0;
	const char *str = s;

	while (*str != sep && *str != '\0')
		++i, str++;

	if (*str == '\0' && sep != '\0')
		return -1;

	return i;
}


