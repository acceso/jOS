
// http://forum.osdev.org/viewtopic.php?f=1&p=167798


#include <stdint.h>
#include <stdio.h>
#include <lib/kernel.h>

#include <kernel/intr.h>
#include <kernel/traps.h>



struct _drive {
	u64 nblocks;

	u64 nread;
	u64 nwritten;
};


#define NCHANNELS 2



static struct {
	u16 iobase;
	u8 irq;

	struct _drive drive[2]; /* drive[0] master, drive[1] slave */

} channels[NCHANNELS];



static void
channel_reset (u8 n)
{

}



__isr__
do_ide (struct intr_frame r)
{
	intr_enter ();


	kprintf ("disk intr!");


	lapic_eoi ();
	intr_exit ();
}



void
init_disks (void)
{

	channels[0].iobase = 0x1f0;
	channels[0].irq = 14 + 32;
	channels[1].iobase = 0x170;
	channels[1].irq = 15 + 32;

	channel_reset (0);
	channel_reset (1);


	intr_install_handler (14, (u64)&do_ide);
	intr_install_handler (15, (u64)&do_ide);
}


