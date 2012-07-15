

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <lib/kernel.h>
#include <lib/spinlocks.h>
#include <kernel/intr.h>
#include <kernel/traps.h>


#include "timer.h"


/* This sucks.... bochs has it connected to ioapic's int 0
 * and qemu (as do most of real computers have it to int 2) */
#define PIT_INT		2


/* interrupts per second. */
#define HZ	1000


static u64 nticks;





#define PIT_C0		0x40
#define PIT_C1		0x41
#define PIT_C2		0x42
/* xx		-> Channel: 00 (0), 01 (1), 10 (2), 11 (read-back)
 *   xx		-> Mode: 01 (low), 10 (high), 11 (low then high)
 *     xxx	-> Operating mode: 000 one shot, 010 periodic timer
 *        x	-> 0 for binary, 1 for bcd (4 digits)
 */
#define PIT_MCR 	0x43

#define PIT_FREQ	1193182 /* hz */


static spin_lock_t pit_calibrated;
static u64 cpu_hz;



__isr__
do_pit (struct intr_frame r)
{
	intr_enter ();

	if (spin_lock_locked (&pit_calibrated)) {
  		cpu_hz = (ticks () - cpu_hz) * 100;
  		kprintf ("Cpu speed calibrated to: %ldMhz\n",
			div (cpu_hz, 1000000));
  	  	spin_lock_unlock (&pit_calibrated);
	} else {
		kprintf ("pit!\n");
	}

	lapic_eoi ();

	intr_exit ();
}



void
nsleep (u64 nsecs)
{
	u64 etsc = ticks () + div (nsecs * cpu_hz, 1000000000);

	while (ticks () < etsc)
		/* TODO: can't yield yet :( */
		asm volatile ("hlt\t\n");
}



void
usleep (u64 usecs)
{
	nsleep (usecs * 1000);
}




static
void init_pit (void)
{
	pit_calibrated = SPIN_LOCK_LOCKED;

	/* 0b00_11_000_0 => ch 0, low and hi, one shot timer, binary */
	/*outb (PIT_MCR, 0x34);*/
	outb (PIT_MCR, 0x30);

	/* 11932 = PIT_FREQ / 10 --> 10 ms */
	outb (PIT_C0, 11932 & 0xff);
	outb (PIT_C0, 11932 >> 8);

	cpu_hz = ticks ();

	idt_set_gate (PIT_INT + 32, (u64)&do_pit, K_CS, GATE_INT);

	spin_lock_lock (&pit_calibrated);
	spin_lock_unlock (&pit_calibrated);
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

	lapictic = lapic_calibration (1000);


	idt_set_gate (LAPIC_TIMER_INTR, (u64)&do_lapic, K_CS, GATE_INT);

	/* Divide configuration register (timer divisor), divide by 1 */
	lapic_write (APIC_TDCR, 0xb);

	/* Selects the interrupt to send when the count reaches zero 
	 * and if periodic or one shot */
	lapic_write (APIC_TLVTE, (1 << 17) | LAPIC_TIMER_INTR);

	/* Count (in bus cycles) and start timer */
	lapic_write (APIC_TICR, lapictic);

	
	return;
}



