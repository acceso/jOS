
/* TODO:
 * - blinking cursor move, see http://osdever.net/bkerndev/Docs/printing.htm
 */


/*
 * Ad-hoc vga driver
 */

#include <types.h>
#include <vga.h>


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



static inline u16 *
addrfrompos(u8 l, u8 c)
{
	return (VGA_BASE + l * 80 + c);
}



static void
scroll(void)
{
	*(u16 *)VGA_BASE = VC('c', C_WHITE, C_BLACK);
	u32 *p = (u32 *)VGA_BASE;

	do {
		*p = *(p + NCOL / 2);
	} while (++p < (u32 *)VGA_END);

	p = (u32 *)(VGA_END - NCOL / 2);
	*p = VC('-', C_WHITE, C_BLACK);

	while (p < (u32 *)(VGA_END - 2))
		*p++ = (VC(' ', C_WHITE, C_BLACK) << 16)
			| VC(' ', C_WHITE, C_BLACK);

	line--;

	return;
}




void
cls(void)
{
	u32 *p = (u32 *)VGA_BASE;
	u32 blank = (VC(' ', C_WHITE, C_BLACK) << 16)
		| VC(' ', C_WHITE, C_BLACK);

	while (p < (u32 *)VGA_END - 2)
		*p++ = blank;

	col = 0;
	line = 0;
}



static void
vga_writechar (const u8 c)
{
	u16 *p = addrfrompos(line, col);

	if (c == '\b')  {
		if (col == 0)
			return;
		
		*p = VC(' ', C_WHITE, C_BLACK);
		col--;
	} else if (c == '\t') {
		if (col >= NCOL - TAB) {
			u16 *p2 = addrfrompos(++line, col = 0);
			
			if (line >= NLIN)
				scroll();

			while (p < p2)
				*p++ = VC(' ', C_WHITE, C_BLACK);

			return;
		}

		do {
			*p++ = VC(' ', C_WHITE, C_BLACK);
		} while (++col % TAB != 0);
	} else if (c == '\n') {
		u16 *p2 = addrfrompos(++line, col = 0);

		if (line >= NLIN)
			scroll();

		while (p < p2)
			*p++ = VC(' ', C_WHITE, C_BLACK);

	} else if (c >= ' ') { /* just if is printable */
		*p = VC(c, C_WHITE, C_BLACK);
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



