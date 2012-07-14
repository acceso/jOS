

#include <lib/stdio.h>
#include <inc/boot.h>

#include "intr.h"
#include "timer.h"
#include "traps.h"


static u64 nticks;


__isr__ static void
do_timer (struct intr_frame r)
{
	intr_enter ();

	nticks++;

	/*kprintf ("tick! %llu :)\n", nticks);*/

	lapic_eoi ();

	intr_exit ();
}


void
init_timer (u32 apictic)
{
	idt_set_gate (TIMER_INTR, (u64)&do_timer, K_CS, GATE_INT);

	/* Lapic divide configuration register (timer divisor), divide by 1 */
	lapic_write (APIC_TDCR, 0xb);

	/* Selects the interrupt to send when the count reaches zero */
	lapic_write (APIC_TLVTE, (1 << 17) | TIMER_INTR);

	/* Count (in bus cycles) */
	lapic_write (APIC_TICR, apictic);

}



