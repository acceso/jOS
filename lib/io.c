
#include <drivers/vga.h>
#include <inc/types.h>

#include "io.h"





void
puts(const char *s)
{
	while (*s)
		vga_writechar(*s++);
}


void
itoa (u64 n, char *s, u8 base)
{
	char *p;
	u64 pn = n;

	if (n < 0) {
		*s++ = '-';
		pn = -n;
	}

	p = s;

	do {
		int rem = pn % base;

		*p++ = (rem < 10) ? rem + '0' : rem + 'a' - 10 ;

	} while (pn /= base);

	*p-- = '\0';

	while (p > s) {
		char tmp = *s;

		*s++ = *p;
		*p-- = tmp;
	}

}





void
kprintf (const char *fmt, ...)
{
	const char *p = fmt;
	va_list ap;

	if (p == NULL)
		return;

	va_start (ap, fmt);

	do {
		if (*p == '\0')
			break;

		if (*p++ != '%') {
			vga_writechar (*(p - 1));
			continue;
		}

		switch (*p) {
		case '%':
			vga_writechar ('%');
			break;
		case 'c':
			vga_writechar ((char) va_arg (ap, int));
			break;
		case 'd':
			{
			char s[22];
			itoa (va_arg (ap, long int), s, 10);
			puts (s);
			break;
			}
		case 's':
			puts (va_arg (ap, char *));
			break;
		case 'x':
			{
			char s[22];
			itoa (va_arg (ap, int), s, 16);
			puts (s);
			break;
			}
		}

		p++;
	} while (1);

	va_end (ap);
}



