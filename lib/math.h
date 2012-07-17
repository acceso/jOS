#ifndef MATH_H
#define MATH_H


#include <stdint.h>



static inline u8
bcd2int (u8 n)
{
	return (n & 0xf) + (n >> 4 & 0xf) * 10;
}



#endif /* MATH_H */


