
#ifndef TIME_H
#define TIME_H


#include <stdint.h>


struct tm {
	s16 tm_sec;	/* seconds 0-59 or 60 for leap seconds */
	s16 tm_min;	/* minutes 0-59 */
	s16 tm_hour;	/* hours 0-23 */
	s16 tm_mday;	/* day of the month 1-31 */
	s16 tm_mon;	/* month 0-11 */
	s16 tm_year;	/* year since 1900 */
	s16 tm_wday;	/* day of the week since Sunday: 0-6 */
	s16 tm_yday;	/* day in the year 0-365 */
	s16 tm_isdst;	/* daylight saving time 1:yes, 0:no, -1 unknown */
};



time_t mktime (struct tm *tim);


#endif /* TIME_H */


