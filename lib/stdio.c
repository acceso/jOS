
#include <drivers/vga.h>
#include <inc/types.h>
#include <lib/string.h>

#include "stdio.h"



void
puts (const char *s)
{
	while (*s)
		vga_writechar (*s++);
}



void
itoa (s64 n, char *s, u8 base)
{
	char *p;
	s64 pn = n;

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
itoau (u64 n, char *s, u8 base)
{
	char *p;
	u64 pn = n;

#if 0
	if (n == 0xffff8000003f9000) {
		char ss[10];
		u8 a = (n & (0xff00000000000000)) >> 56;
		u8 b = (n & (0x00ff000000000000)) >> 48;
		u8 c = (n & (0x0000ff0000000000)) >> 40;
		u8 d = (n & (0x000000ff00000000)) >> 32;
		u8 e = (n & (0x00000000ff000000)) >> 24;
		u8 f = (n & (0x0000000000ff0000)) >> 16;
		u8 g = (n & (0x000000000000ff00)) >>  8;
		u8 h = (n & (0x00000000000000ff)) >>  0;
		puts ("\n");
		puts ("->"); itoa (a, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (b, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (c, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (d, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (e, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (f, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (g, ss, 10); puts (ss); puts ("<-\n");
		puts ("->"); itoa (h, ss, 10); puts (ss); puts ("<-\n");
	}
#endif

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





u64
atoi (const char *s)
{
	u64 n = 0;

	while (is_digit (*s++)) {
		n *= 10;
		n += *(s - 1) - '0';
	}

	return n;
}



#define KP_LEFTJUST	1
#define KP_SIGN		2
#define KP_ZEROPAD	4

#define KP_LONG		8
#define KP_LONGLONG	16

#define KP_DEFWIDTH	16


char buf[2048];



/* 
 * Spec: http://www.opengroup.org/onlinepubs/000095399/functions/printf.html
 * partly implemented.
 * TODO: not bound checking for "buf"!
 */
void
kprintf (const char *fmt, ...)
{
	const char *p = fmt;
	char *b = buf;
	va_list ap;
	u32 flags;
	u8 width;

	if (p == NULL)
		return;

	va_start (ap, fmt);

	do {
		if (*p == '\0') {
			*b++ = '\0';
			break;
		}

		if (*p++ != '%') {
			*b++ = *(p - 1);
			continue;
		}

		if (*p == '%') {
			*b++ = *p++;
			continue;
		}


		flags = 0;
		
		do {
			if (*p == '-')
				flags |= KP_LEFTJUST;
			else if (*p == '+')
				flags |= KP_SIGN;
			/* Too mucho f a hassle... */
			/*else if (*p == '0')
				flags |= KP_ZEROPAD; */
			else
				break;

			p++;

		} while (1);


		if (is_digit (*p))
			width = atoi (p);
		else
			width = 0;


		while (is_digit (*p))
			p++;


		/* no precision because no floating point yet */


		if (*p == 'l') {
			p++;
			if (*p == 'l') {
				flags |= KP_LONGLONG;
				p++;
			} else
				flags |= KP_LONG;
		}


		switch (*p) {
		case 'c':
			*b++ = ((char) va_arg (ap, int));
			break;
		case 'd':
		case 'i':
		case 'p':
		case 'u':
		case 'x':
			{
			char s[22];
			char *ps = s;
			s16 l;
			char c;
			u8 base = 10;

			if (*p == 'x' || *p == 'p')
				base = 16;

			/* First, let's get a string representation: */
			if (*p == 'u' || *p == 'p') {
				if (flags & KP_LONGLONG)
					itoau (va_arg (ap, unsigned long long int), s, base);
				else if (flags & KP_LONG)
					itoau (va_arg (ap, unsigned long int), s, base);
				else
					itoau (va_arg (ap, unsigned int), s, base);
			} else {
				if (flags & KP_LONGLONG)
					itoa (va_arg (ap, long long int), s, base);
				else if (flags & KP_LONG)
					itoa (va_arg (ap, long int), s, base);
				else
					itoa (va_arg (ap, int), s, base);
			}


			/* The lenght of the string */
			l = strlen (s);
			/* Plus one if the positive sign is wanted */
			if (flags & KP_SIGN && s[0] != '-')
				l++;
			/* ... plus two for the string: '0x' */
			if (*p == 'p')
				l += 2;


			/* The filling character: */
			if (flags & KP_ZEROPAD)
				c = 0;
			else
				c = ' ';


			/* If not left adjusted and the width is greater than the 
			 * generated string: */
			if ((flags & KP_LEFTJUST) == 0 && width - l > 0) {
				l = width - l;
				while (l--)
					*b++ = c;
			}


			/* The sign: */
			if (s[0] == '-') {
				*b++ = '-';
				ps++;
			} else if (flags & KP_SIGN)
				*b++ = '+';

			if (*p == 'p') {
				*b++ = '0';
				*b++ = 'x';
			}

			/* Copy the string */
			while (*ps != '\0')
				*b++ = *ps++;


			/* Fill the right side if specified */
			if ((flags & KP_LEFTJUST) && width - l > 0) {
				l = width - l;
				while (l--)
					*b++ = c;
			}

			}
			break;
		case 's':
			{
			char *s;

			s = va_arg (ap, char *);
			while (*s)
				*b++ = *s++;

			}
			break;
		}

		p++;
	} while (1);


	puts (buf);


	va_end (ap);
}



void
outb (u16 port, u8 val)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}



u8
inb (u16 port)
{
	u8 ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}



u16
inw(u16 port)
{
	u16 ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
} 



