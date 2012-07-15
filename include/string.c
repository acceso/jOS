

#include <stdint.h>




s8
strcmp (const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s1 == *s2)
		s1++, s2++;

	if (*s1 == *s2)
		return 0;

	return (*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : 1;
}



s8
strncmp (const char *s1, const char *s2, u16 l)
{
	while (--l > 0 && *s1 == *s2)
		s1++, s2++;

	if (l <= 0 || *s1 == '\0')
		return 0;

	return (*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : 1;
}



void
strcpy (char *src, char *dst)
{
	while (*src)
		*dst++ = *src++;
}



void
strncpy (char *src, char *dst, s16 l)
{
	while (*src) {
		if (--l < 0)
			break;
		*dst++ = *src++;
	}
}



u16
strlen (char *s)
{
	u16 i = 0;

	while (*s++)
		++i;

	return i;
}




