
#ifndef DRIVERS_PIT_H
#define DRIVERS_PIT_H


#include <stdint.h>





u64 tsc_calibration_withpit(u64 *loops);
void init_pit(void *intr_handler, u32 hz);


#endif /* DRIVERS_PIT_H */

