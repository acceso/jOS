#ifndef TIMER_H
#define TIMER_H


#include <stdint.h>


static inline u64
ticks (void)
{
	u32 a;
	u64 d;

	asm volatile ("rdtsc\n" : "=a"(a), "=d"(d));

	return (d << 32) | a;
}


void nsleep (u64 nsecs);
void usleep (u64 usecs);

void init_timers (void);




#endif /* TIMER_H */

