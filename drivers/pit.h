
#ifndef PIT_H
#define PIT_H


#include <stdint.h>





u64 tsc_calibration_withpit (u64 *loops);
void init_pit (u64 intr_handler, u32 hz);


#endif /* PIT_H */

