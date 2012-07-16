#ifndef TIMERS_H
#define TIMERS_H


#include <stdint.h>
#include <stdlib.h>

#include <lib/cpu.h>

#include <kernel/cpu.h>



static inline u64
ticks (void)
{
	u32 a;
	u32 d;

	/* TODO: Needs one of:
		"lfence\n\t"
		"mfence\n\t"
		"sfence\n\t"
	 */

	asm volatile (
		"rdtsc\n\t"
		: "=a"(a), "=d"(d));

	return ((u64)d << 32) | a;
}




extern struct _sys sys;



static inline void
sleep_generic (u64 n, u32 d)
{
	u64 future_ticks = ticks () + n / d;

	while (ticks () < future_ticks)
		yield ();

}


static inline void
nsleep (u32 nsecs)
{
	sleep_generic (nsecs * sys.cpu[0].hz, 1000000000);
}



static inline void
usleep (u32 usecs)
{
	sleep_generic (usecs * sys.cpu[0].hz, 1000000);
}



static inline void
msleep (u32 msecs)
{
	sleep_generic (msecs * sys.cpu[0].hz, 1000);
}



void init_timers (void);




#endif /* TIMERS_H */

