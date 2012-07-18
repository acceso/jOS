
#include <stdint.h>
#include <stdio.h>

#include <drivers/lapictim.h>
#include <drivers/pit.h>
#include <drivers/rtc.h>

#include <kernel/traps.h>

#include "timers.h"




/* Kernel ticks per second: */
#define HZ		100
/* 65536: --> 1193181.66 / 65536 => 18.2065 hz
 * For 10 ms (100 hz): 1193181.66 / x => 100 hz, x == 11932 */
#define PIT_FREQ	11932



/* Timer tick duration: */
volatile u64 jiffies;
static u64 loops_per_jiffy;




__isr__
do_pit (struct intr_frame r)
{
	intr_enter ();


	jiffies++;
	/*if (jiffies % 10 == 0)
		kprintf (".");*/


	lapic_eoi ();

	intr_exit ();
}




__isr__
do_lapictim (struct intr_frame r)
{
	intr_enter ();

	/*kprintf ("tick! %llu ", ticks ());*/

	lapic_eoi ();

	intr_exit ();
}



static void
tsc_calibration (void)
{
	u64 hz, loops;
	u8 i;

	sys.cpu[0].hz = (u64)-1;
	loops_per_jiffy = (u64)-1;

	/* Best of four :) */
	i = 4;

	while (i--) {
		hz = tsc_calibration_withpit (&loops);
		if (hz < sys.cpu[0].hz)
			sys.cpu[0].hz = hz;
		if (loops < loops_per_jiffy)
			loops_per_jiffy = loops;
	}

	kprintf ("Cpu speed calibrated to: %ldMhz, %ld BogoMIPS\n",
		sys.cpu[0].hz / 1000000, loops_per_jiffy);

}





void
init_timers (void)
{

	tsc_calibration ();

	init_pit (do_pit, HZ);

	init_lapic_timer (do_lapictim, HZ);

	init_rtc ();
	
	return;
}




