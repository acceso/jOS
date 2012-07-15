

#define VERSION "0.0.0.10"


#include <stdint.h>
#include <stdio.h>
#include <drivers/disk.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <drivers/vga.h>

#include <kernel/mm.h>
#include <kernel/intr.h>
#include <kernel/acpi.h>
#include <kernel/traps.h>



u16
kmain (void)
{
	cls ();
	puts ("Booting jOS kernel " VERSION "!\n");

	init_memory ();
	init_acpi ();
	init_exceptions ();
	init_interrupts ();

	/* Abrimos la caja de pandora: */
	interrupts_enable ();

	init_keyboard ();
	init_timers ();

#if 0
	init_disks ();

	
	{
	u64 i = 0;
	while (1) {
		/*usleep (1000000);*/
		kprintf ("ejem! %d\n", i++);
	}
	}
#endif
	return 0;
}



