
#include "kmain.h"

void
memcpy (char *dst, const char *src, u32 count)
{
	for(; count != 0; count--)
		*dst++ = *src++;

}

void
memset (char *dst, const char c, unsigned count)
{
	for(; count != 0; count--)
		*dst++ = c;

	return;
}

int 
jmain (void)
{
	static const char msg[] = "H o l aaaaaaaa!! 64 :=)";
	unsigned vga_fb_adr = 0xB8000;

	/* CLS :) */
	memset ((char *)(vga_fb_adr), 0, 320*200);

	memcpy ((char *)(vga_fb_adr), msg, sizeof(msg) - 1);

	/* return to KSTART.ASM, which will freeze */
	return 0;
}



