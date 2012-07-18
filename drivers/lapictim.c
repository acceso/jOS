


#include <stdint.h>

#include <kernel/intr.h>
#include <kernel/timers.h>
#include <kernel/traps.h>

#include <lib/debug.h>




static u32
lapic_get_current_count (void)
{
	return lapic_read (APIC_TCCR);
}



void
init_lapic_timer (void *intr_handler, u32 hz)
{
	u32 lapictic;

	intr_install_handler (LAPIC_TIMER_INTR, intr_handler);


	/* Divide configuration register (timer divisor), divide by 1 */
	lapic_write (APIC_TDCR, 0xb);

	/* Selects the interrupt to send when the count reaches zero 
	 * and if periodic or one shot */
	lapic_write (APIC_TLVTE, (1 << 17) | LAPIC_TIMER_INTR);

	/* Count (in bus cycles) and start timer.
	 * shouldn't fire, it's just to get the count, see below */
	lapic_write (APIC_TICR, (u32)-1);

	lapictic = lapic_get_current_count ();
	msleep (1);
	lapictic -= lapic_get_current_count ();

	sys.busspeed = lapictic;

	/* This timer gives lapictic*1000 ticks on 1 sec.
	 * The user wants 'hz' ticks every second. So:
	 * x = lapictic * 1000 / hz
	 */
	lapic_write (APIC_TICR, div (lapictic * 1000, hz));



	kprintf ("External clock: %ldMhz\n", div (lapictic, 1000));


	return;
}




