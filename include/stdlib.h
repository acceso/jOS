
#ifndef STDLIB_H
#define STDLIB_H


#include <stdint.h>



static u64 inline
div (u64 n, u32 d)
{
	u32 q, r;

	asm (	"divl %[n]\n\t"
		: "=d" (r), "=a" (q)
		: "0" (n >> 32), "1" (n), [n] "rm" (d));

	return q;
}



u64 atoi (const char *s);




#endif /* STDLIB_H */


