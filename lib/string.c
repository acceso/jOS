

#include <inc/types.h>


u16
strlen (char *s)
{
	u16 i = 0;

	while (*s++)
		++i;

	return i;
}


void
strcpy (char *src, char *dst)
{
	while (*src)
		*dst++ = *src++;
}


void
memset (void *p, u8 c, size_t n)
{

}







