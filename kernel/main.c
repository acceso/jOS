

#define VERSION "0.0.0.22"


#include <stdint.h>
#include <stdio.h>

#include <lib/debug.h>
#include <mm/mm.h>

#include <drivers/block.h>
#include <drivers/device.h>
#include <drivers/disk.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>

#include <vm/cpu.h>
#include <kernel/acpi.h>
#include <kernel/intr.h>
#include <kernel/task.h>
#include <kernel/timers.h>
#include <kernel/traps.h>

#include <fs/fs.h>


static dev_t root = { .major = BMAJOR_HD, .minor = 1 };

struct task *current;



u16
kmain (void)
{
	cls ();
	kputs ("Booting jOS kernel " VERSION "!\n");

	init_acpi ();
	init_exceptions ();
	init_interrupts ();
	init_memory ();
	init_cpu ();

	interrupts_enable ();

	init_timers ();
	init_keyboard ();

	init_disks ();
	init_fs (&root);

	current = init_task ();

elf_exec ("/elf1");

	while (1)
		;

	return 0;
}



