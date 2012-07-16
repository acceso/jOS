

#define VERSION "0.0.0.14"


#include <stdint.h>
#include <stdio.h>

#include <lib/debug.h>
#include <mm/mm.h>

#include <drivers/disk.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>

#include <kernel/acpi.h>
#include <kernel/cpu.h>
#include <kernel/intr.h>
#include <kernel/timers.h>
#include <kernel/traps.h>




u16
kmain (void)
{
	cls ();
	puts ("Booting jOS kernel " VERSION "!\n");

	init_cpu ();
	init_acpi ();
	init_exceptions ();
	init_interrupts ();
	init_memory ();

	/* Abrimos la caja de pandora: */
	interrupts_enable ();

	init_timers ();
	init_keyboard ();

#if 0
	init_disks ();

#endif
	{
	int i = 6;
	while (--i) {
		kprintf ("%d", i);
		msleep (1000);
	}
	}
	
	/* This should be temporal, just until we could fork init,
	 * clone threads, tty layer... */
	init_input ();

	return 0;
}



