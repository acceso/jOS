
#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>
#include <lib/kernel.h>
#include <kernel/traps.h>
#include <kernel/intr.h>



static struct {
	u8 shift:1;
	u8 ctrl:1;
} keyb;


/* TODO:
 * - backspace
 */


/* Info: http://www.ee.bgu.ac.il/~microlab/MicroLab/Labs/ScanCodes.htm
 * Bochs adapts the keycodes to spanish?? Should fix this...
 */
char kb_en[256] = "\0\0"
	"1234567890-=\b"
	"\tqwertyuiop[]\n"
	"\0asdfghjkl;'`"
	"\0\\zxcvbnm,./\0"
	"\0\0 \0\0\0\0\0"
	"\0\0\0\0\0\0\0\0"
	"\0\0\0-\0\0\0+"
	"\0\0\0\0\0\0\0<";
char kb_en_caps[256] = "\0\0"
	"!@#$%^&*()_+\b"
	"\tQWERTYUIOP{}\n"
	"\0ASDFGHJKL:\"~"
	"\0|zxcvbnm<>?\0"
	"\0\0 \0\0\0\0\0"
	"\0\0\0\0\0\0\0\0"
	"\0\0\0-\0\0\0+"
	"\0\0\0\0\0\0\0>";



__isr__
do_keyboard (struct intr_frame r)
{
	intr_enter ();

	u8 scancode = inb (0x60);


#if 0 /* used for debugging */
	kprintf ("->%d<-\n", scancode);
#else

	switch (scancode) {
	case 0x2a: /* Left shift */
	case 0x36: /* Right shift */
		keyb.shift = 1;
		break;
	case 0xaa: /* Left shift */
	case 0xb6: /* Right shift */
		keyb.shift = 0;
		break;
	default:
		{
		char c;
		
		/* 0b1000_0000 means key release (break) */
		if (scancode & 0x80)
			break;

		if (keyb.shift)
			c = kb_en_caps[scancode];
		else
			c = kb_en[scancode];

		switch (c) {
		default:
			kprintf ("%c", c);
		}

		}
	}
#endif

	lapic_eoi ();
	intr_exit ();
}




void
init_keyboard (void)
{
	intr_install_handler (1, (u64)&do_keyboard);


	return;
}



