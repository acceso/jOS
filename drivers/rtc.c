

/* TODO: this is noway complete... will work for now, remaining stuff:
 * * year will always be 20xx.. shoudn't be a problem for ~90 years :)
 * * code the typical functions: time, gettimeofday, localtime....
 *   it's work and they're not needed for now
 * Info: 
 * http://www-ivs.cs.uni-magdeburg.de/~zbrog/asm/cmos.html
 * http://www.nondot.org/sabre/os/files/MiscHW/RealtimeClockFAQ.txt
 * http://www.compuphase.com/int70.txt
 * It doesn't come that easy, uh?
 */



#include <stdint.h>
#include <time.h>

#include <lib/math.h>
#include <sys/io.h>

#include <kernel/intr.h>
#include <kernel/traps.h>
#include <kernel/timers.h>



#define RTCHZ		1024


#define RTC_INDEX	0x70
#define RTC_DATA	0x71




static inline u8
rtc_read (u8 port)
{
	/* The high order bit disables NMI 
	 * Could leave the cmos inconsistent if unset */
	outb (RTC_INDEX, port | (1<<7));

	return inb (RTC_DATA);
}


static void
rtc_write (u8 port, u8 data)
{
	outb (RTC_INDEX, port | (1<<7));
	outb (RTC_DATA, data);
}



void
rtc_get_tm (struct tm *tim)
{	
	u16 daysmonth[] = { 0, 0, 31, 59, 90, 120, 151, 
		181, 212, 243, 273, 304, 334 };

	/* When this bit is set, rtc is updating the count. 
	 * It gives us 244 usecs */
	while ((rtc_read (0xa) & 0x80) != 0)
		;

	tim->tm_sec = rtc_read (0);
	tim->tm_min = rtc_read (2);
	tim->tm_hour = rtc_read (4);
	tim->tm_mday = rtc_read (7);
	tim->tm_mon = rtc_read (8);
	tim->tm_year = rtc_read (9) + 2000;
		/* Century will be assumed 20. 
		 * This code doesn't always work: */
		/* + bcd2int (rtc_read (0x32)) * 100; */
	tim->tm_wday = rtc_read (6) - 1;

	tim->tm_yday = daysmonth[tim->tm_mon] + tim->tm_mday;
}



static volatile u64 unixtime;
static volatile u16 hznum;

__isr__
do_rtc (struct intr_frame r)
{
        intr_enter ();

	struct tm tim;

	/* The RTC needs a read to port 0xc, 
	 * otherwise it sends no more interrupts 
	 * x          : interrupts enabled in reg b
	 *  x         : periodic interrupt has ocurred
	 *   x        : alarm interrupt
	 *    x       : update-ended alarm
	 *     xxxx   : always 0 */
	rtc_read (0xc);


	hznum++;
	if (hznum == RTCHZ) {
		hznum = 0;
		unixtime++;
		/* Once every hour, recalibrate the unix time */
		if (unixtime % 3600) {
			rtc_get_tm (&tim);
			unixtime = mktime (&tim);
		}
	}


	lapic_eoi ();

	intr_exit ();
}





void
init_rtc (void)
{
	struct tm tim;


	intr_install_handler (8, do_rtc);

	interrupts_disable (); /* --------------- */


	/*   x        : update in progress bit
	 *    xxx     : leave alone: 0b010 means 32768Hz
	 *       xxxx : 32768Hz >> (0bxxxx - 1), defaults to: 0b0110, 1024Hz
	 * Defaults are good enough: */
	/* rtc_write (0xa, 0b); */

	/*    x       : PIE: periodic interrupt enable
	 *     xxx    : alarm, update-ended interrupt, square wave
	 *        x   : binary mode (1) or bcd (0)
	 *         x  : 24 hour format (1) or 12 (0)
	 *          x : daylight savings enable
	 */
	rtc_write (0xb, rtc_read (0xb) | 0b1000110);


	interrupts_enable (); /* --------------- */


	/* FIXME: It takes some time to have effect.. ¿? */
	msleep (500);

	rtc_get_tm (&tim);
	/* The int handler is already running, this should overwrite it */
	unixtime = mktime (&tim);

	/* TODO: zero pad this... */
	kprintf ("System clock set to: %d-%d-%d %d:%d:%d (unix: %lld)\n", 
		tim.tm_year, tim.tm_mon, tim.tm_mday, tim.tm_hour, tim.tm_min,
		tim.tm_sec, unixtime);

	return;
}



