

#define VERSION "0.0.0.15"


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

#include <fs/fs.h>



u16
kmain (void)
{
	cls ();
	kputs ("Booting jOS kernel " VERSION "!\n");

	init_cpu ();
	init_acpi ();
	init_exceptions ();
	init_interrupts ();
	init_memory ();

	/* Abrimos la caja de pandora: */
	interrupts_enable ();

	init_timers ();
	init_keyboard ();

	init_disks ();
	//init_fs ();


	return 0;
}



