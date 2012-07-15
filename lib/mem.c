

#include <stdio.h>
#include <stdint.h>




/* I could probably do better :).
 * Aligns an address to a power */
void *
align_to (void *a, u32 p)
{
	u32 q = p - 1;
	u32 i = 0;

	/* P Not a power of two: */
	if (p == 0 || (p & (p -1)) != 0)
		return 0;

	if ((u64)a % p == 0)
		return a;

	do {
		i++;
	} while (q >>= 1);


	return (void *)((((long)a >> i) << i) + p);
}




