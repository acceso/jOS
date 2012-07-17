
#include <stdint.h>

#include <sys/io.h>

#include <kernel/intr.h>
#include <kernel/timers.h>
#include <kernel/traps.h>




/* xx	    -> Channel: 00 (0), 01 (1), 10 (2), 11 (read-back)
 *   xx	    -> Mode: 01 (low), 10 (high), 11 (low then high)
 *	       00 (latch count value cmd)
 *     xxx  -> Operating mode: 000 intr on tc, 001 hw re-triggerable one-show,
 *	       010 rate generator, 
 *        x -> 0 for binary, 1 for bcd (4 digits)
 */
#define PIT_MCR 	0x43

#define PIT_C2GATE	0x61



static u16
pit_read_count (u8 chan)
{
	/* Latches the current count value */
	outb (PIT_MCR, chan << 6);

	/* Read pit count: */
	/* NOTE: as of 05-2010 (ubuntu lucid), qemu+kvm loses interrupts
	 * when the count is read "too many" times (~450-480 on my system). 
	 * I'd bet there's a bug on kvm but I have no time to check it out,
	 * I'll rmmod kvm_intel for now */
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



u64
tsc_calibration_withpit (u64 *loops)
{
	u64 cpu_hz, l = 0;

	/* Enable channel 2 but don't route it to the speaker (I believe,
	 * see: http://www.sat.dundee.ac.uk/~psc/pctim003.txt , 7.5) */
	outb (PIT_C2GATE, (inb (PIT_C2GATE) & ~0b10) | 0b1);

	pit_set_timer (2, 0b11, 0b010, 0);


	cpu_hz = ticks ();

	/* Bogomips are meaningless, aren't they? :D 
	 * pit_read_count introduces much more delay than the loop */
	while (pit_read_count (2) > (u16)-1 - 11932)
		l++; /* nop */

	cpu_hz = (ticks () - cpu_hz) * 100;


	/* Stop the channel */
	outb (PIT_C2GATE, inb (PIT_C2GATE) & ~0b11);

	*loops = l;

	return cpu_hz;

}




void
init_pit (u64 intr_handler, u32 hz)
{
	pit_set_timer (0, 0b11, 0b010, hz);

	intr_install_handler (0, intr_handler);
}




