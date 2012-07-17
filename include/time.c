

#include <stdint.h>

#include <drivers/rtc.h>

#include "time.h"



#define DAYSECS	86400


u64
mktime (struct tm *tim)
{
	/* It should work with dates >= 2000 and < 2100. */
	return  946684800 + /* This is 2000-01-01 00:00, our 0. */
		DAYSECS * 365 * (tim->tm_year - 2000) + /* Years */
		DAYSECS * ((tim->tm_year - 2000 + 3) / 4) + /* Leap years */
		DAYSECS * (tim->tm_yday - 1) +	/* Days in this year */
		3600 * tim->tm_hour + 60 * tim->tm_min + tim->tm_sec;


}



