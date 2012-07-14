

#define VERSION "0.0.0.8"


#include <inc/types.h>
#include <kernel/intr.h>
#include <kernel/timer.h>
#include <kernel/traps.h>
#include <drivers/vga.h>
#include <lib/stdio.h>

#include "mm.h"



u16
kmain (void)
{
	cls ();
	puts ("Booting jOS kernel " VERSION "!\n");

	init_memory ();
	init_exceptions ();
	init_interrupts ();
	init_timer (100000000);

	
	/*asm volatile ("int $0x03");*/
	/*asm volatile ("int $0x04");*/


	/* Abrimos la caja de pandora: */
	asm volatile ("sti\n\t");


	return 0;
}



