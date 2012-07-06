
/* TODO:
 * - blinking cursor move, see http://osdever.net/bkerndev/Docs/printing.htm
 */


/*
 * Ad-hoc vga driver
 */

#include <types.h>
#include "vga.h"


#define C_BLACK         0x0
#define C_BLUE          0x1
#define C_GREEN         0x2
#define C_CYAN          0x3
#define C_RED           0x4
#define C_MAGENTA       0x5
#define C_BROWN         0x6
#define C_LIGHT_GREY    0x7
#define C_DARK_GREY     0x8
#define C_LIGHT_BLUE    0x9
#define C_LIGHT_GREEN   0xA
#define C_LIGHT_CYAN    0xB
#define C_LIGHT_RED     0xC
#define C_LIGHT_MAGENTA 0xD
#define C_LIGHT_BROWN   0xE
#define C_WHITE         0xF

#define TAB	8

/* 4 fg, 4 bg, 8 char */
#define VC(_c, _fg, _bg)        ((_c) | (((_fg) | (_bg) << 4) << 8))


#define VGA_BASE ((u16 *)0xB8000L)
#define VGA_END  ((u16 *)0xb8ff0L)

#define NLIN 25
#define NCOL 80


static u8 line;
static u8 col;

static u8 c_front = C_LIGHT_GREY;
static u8 c_back = C_BLACK;



static inline u16 *
addrfrompos(u8 l, u8 c)
{
	return (VGA_BASE + l * 80 + c);
}



static void
scroll(void)
{
	*(u16 *)VGA_BASE = VC('c', c_front, c_back);
	u32 *p = (u32 *)VGA_BASE;

	do {
		*p = *(p + NCOL / 2);
	} while (++p < (u32 *)VGA_END);

	p = (u32 *)(VGA_END - NCOL / 2);
	*p = VC('-', c_front, c_back);

	while (p < (u32 *)(VGA_END - 2))
		*p++ = (VC(' ', c_front, c_back) << 16)
			| VC(' ', c_front, c_back);

	line--;

	return;
}




void
cls(void)
{
	u32 *p = (u32 *)VGA_BASE;
	u32 blank = (VC(' ', c_front, c_back) << 16)
		| VC(' ', c_front, c_back);

	while (p < (u32 *)VGA_END - 2)
		*p++ = blank;

	col = 0;
	line = 0;

	return;
}



static void
vga_writechar (const u8 c)
{
	u16 *p = addrfrompos(line, col);

	if (c == '\b')  {
		if (col == 0)
			return;
		
		*p = VC(' ', c_front, c_back);
		col--;
	} else if (c == '\t') {
		if (col >= NCOL - TAB) {
			u16 *p2 = addrfrompos(++line, col = 0);
			
			if (line >= NLIN)
				scroll();

			while (p < p2)
				*p++ = VC(' ', c_front, c_back);

			return;
		}

		do {
			*p++ = VC(' ', c_front, c_back);
		} while (++col % TAB != 0);
	} else if (c == '\n') {
		u16 *p2 = addrfrompos(++line, col = 0);

		if (line >= NLIN)
			scroll();

		while (p < p2)
			*p++ = VC(' ', c_front, c_back);

	} else if (c >= ' ') { /* just if is printable */
		*p = VC(c, c_front, c_back);
		col++;

		if (col >= NCOL - 1) {
			if (line >= NLIN - 1)
				scroll();
			col = 0;
			line++;
		}
	}

	return;

}




void
puts(const char *s)
{
	while (*s)
		vga_writechar(*s++);
}




/* TODO: reemplazar esta función */
static void
itoa (char *buf, int base, int d)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head. */
	if (base == 'd' && d < 0)
	{
		*p++ = '-';
		buf++;
		ud = -d;
	}
	else if (base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0. */
	do
	{
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}
	while (ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}





/* TODO: reemplazar esta función también */
void
printf(const char *fmt, ...)
{
	char **arg = (char **) &fmt;
	int c;
	char buf[20];
	char *p;

	arg++;

	while ((c = *fmt++) != 0)
	{
		if (c != '%') {
			vga_writechar (c);
			continue;
		}

		c = *fmt++;
		switch (c)
		{
			case 'd':
			case 'u':
			case 'x':
				itoa (buf, c, *((int *) arg++));
				p = buf;
				goto string;
				break;

			case 's':
				p = *arg++;
				if (! p)
					p = "(null)";

string:
				while (*p)
					vga_writechar (*p++);
				break;

			default:
				vga_writechar (*((int *) arg++));
				break;
		}
	}


}


