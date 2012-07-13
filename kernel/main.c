

#define VERSION "0.0.0.7"


#include <inc/types.h>
#include <kernel/traps.h>
#include <drivers/vga.h>
#include <lib/stdio.h>

#include "mm.h"



u16
kmain (void)
{
	cls ();
	puts ("Booting jOS kernel " VERSION "!\n");

	init_exceptions ();
	init_memory ();


	/*init_pic ();*/
	/*init_timer ();*/

	
	/*asm volatile ("int $0x03");*/
	/*asm volatile ("int $0x04");*/


	/* Abrimos la caja de pandora: */
	/*asm volatile ("sti\n");*/


	return 0;
}



