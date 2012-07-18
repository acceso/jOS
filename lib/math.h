#ifndef LIB_MATH_H
#define LIB_MATH_H


#include <stdint.h>




#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


static inline u8
bcd2int (u8 n)
{
	return (n & 0xf) + (n >> 4 & 0xf) * 10;
}


/* Given a number x, returns y, such x = 2^y
 * It's like this python code: math.floor(math.log(x-1, 2)) + 1 
 * but it works for 0 and 1 */
static inline u64
num2power (u64 x)
{
	unsigned power = 1;

	/*  2^0 == 1, good enough. */
	if (x == 0 || x == 1)
		return 0;

	x--;

	while (x >>= 1)
		power++;

	return power;
}


#endif /* LIB_MATH_H */


