
#include <inc/types.h>
#include <inc/boot.h>
#include <kernel/traps.h>
#include <kernel/intr.h>
#include <lib/stdio.h>


static struct {
	u8 shift:1;
	u8 ctrl:1;
} keyb;


char kb_en[] = { 0,
		27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
		'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
		0 /* Control */ , 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
		'\'', '`',   0,
		'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
		0 /*alt*/ , ' ', 0 /* caps lock */,
		0 /* f1 */, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* f10 */ ,
		0 /* num lock */, 0 /* scroll lock */, 0 /* home */,
		0 /* up */, 0 /* page up*/,
		'-',
		0 /* left */, 0,
		0 /* right */,
		'+',
		0 /*end */,
		0 /* down */, 0 /* page down */,
		0 /* insert */, 0 /* delete */,
		0, 0, 0, 0 /* f11 */, 0 /* f12 */,
		0,
		};


__isr__ static void
do_keyboard (struct intr_frame r)
{
	intr_enter ();

	u8 scancode = inb (0x60);
	
	switch (scancode) {
	case 0x2a:
		keyb.shift = 1;
		break;
	case 0xaa:
		keyb.shift = 0;
		break;
	default:
		/* 0b1000_0000 means key release (break) */
		if (scancode & 0x80)
			break;

		kprintf ("%c", kb_en[scancode]);
	}



	lapic_eoi ();
	intr_exit ();
}




void
init_keyboard (void)
{
	idt_set_gate (33, (u64)&do_keyboard, K_CS, GATE_INT);


	return;
}




