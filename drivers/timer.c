

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <lib/cpu.h>
#include <lib/kernel.h>
#include <lib/spinlocks.h>
#include <kernel/cpu.h>
#include <kernel/intr.h>
#include <kernel/traps.h>


#include "timer.h"



/* interrupts per second. */
#define HZ	1000


static u64 nticks;





/* xx	    -> Channel: 00 (0), 01 (1), 10 (2), 11 (read-back)
 *   xx	    -> Mode: 01 (low), 10 (high), 11 (low then high)
 *	       00 (latch count value cmd)
 *     xxx  -> Operating mode: 000 intr on tc, 001 hw re-triggerable one-show,
 *	       010 rate generator, 
 *        x -> 0 for binary, 1 for bcd (4 digits)
 */
#define PIT_MCR 	0x43

#define PIT_C2GATE	0x61


extern struct _sys sys;
static u64 cpu_hz;



void
nsleep (u64 nsecs)
{
	u64 etsc = ticks () + div (nsecs * cpu_hz, 1000000000);

	while (ticks () < etsc)
		/* TODO: can't yield yet :( */
		yield ();
}



void
usleep (u64 usecs)
{
	nsleep (usecs * 1000);
}



void
msleep (u64 msecs)
{
	usleep (msecs * 1000);
}



__isr__
do_pit (struct intr_frame r)
{
	intr_enter ();

	/*kprintf ("pit!\n");*/

	lapic_eoi ();

	intr_exit ();
}



static u16
pit_read_count (u8 chan)
{
	/* Latches the current count value */
	outb (PIT_MCR, chan << 6);

	/* Read pit count: */
	return inb (0x40 + chan) | inb (0x40 + chan) << 8;
}



static
void pit_set_timer (u8 channel, u8 mode, u8 opmode, u16 count)
{
	interrupts_disable (); /* --------------- */


	outb (PIT_MCR, (channel << 6) | (mode << 4) | (opmode << 1));

	if (mode & 0b01)
		outb (0x40 + channel, count & 0xff);
	if (mode & 0b10)
		outb (0x40 + channel, count >> 8);


	interrupts_enable (); /* --------------- */
}



static
void tsc_calibration_withpit (u8 cpu)
{
	/* Enable channel 2 but no speaker */
	/*outb (PIT_C2GATE, (inb (PIT_C2GATE) & ~0b10) | 0b1);*/

	pit_set_timer (2, 0b11, 0b010, 0);

	cpu_hz = ticks ();

	while (pit_read_count (2) > (u16)-1 - 11932)
		;

	cpu_hz = (ticks () - cpu_hz) * 100;

	/* Stop the counter */
	outb (PIT_C2GATE, 0);


	sys.cpu[cpu].hz = cpu_hz;
}



static
void init_pit (void)
{
	/* 65536: --> 1193181.66 / 65536 => 18.2065 hz
	 * We want 10 ms (100 hz) so: 1193181.66 / x => 100 hz, x == 11932 */
	pit_set_timer (0, 0b11, 0b000, 11932);

	intr_install_handler (0, (u64)&do_pit);



	tsc_calibration_withpit (0);
	kprintf ("Cpu speed calibrated to: %ldMhz\n", div (cpu_hz, 1000000));


}





__isr__
do_lapic (struct intr_frame r)
{
	intr_enter ();

	nticks++;

	/*kprintf ("tick! (%llu) %llu \n", nticks, ticks ());*/

	lapic_eoi ();

	intr_exit ();
}



static u64
lapic_calibration (u64 usecs)
{
	return 1000000000;
}




void
init_timers (void)
{
	u32 lapictic;


	init_pit ();
return;


	lapictic = lapic_calibration (1000);


	intr_install_handler (LAPIC_TIMER_INTR, (u64)&do_lapic);


	/* Divide configuration register (timer divisor), divide by 1 */
	lapic_write (APIC_TDCR, 0xb);

	/* Selects the interrupt to send when the count reaches zero 
	 * and if periodic or one shot */
	lapic_write (APIC_TLVTE, (1 << 17) | LAPIC_TIMER_INTR);

	/* Count (in bus cycles) and start timer */
	lapic_write (APIC_TICR, lapictic);

	
	return;
}




