
#ifndef DRIVERS_RTC_H
#define DRIVERS_RTC_H


#include <time.h>



void rtc_get_tm (struct tm *tim);
void init_rtc (void);


#endif /* DRIVERS_RTC_H */

