

#include <stdint.h>
#include <ctype.h>



u64
atoi (const char *s)
{
	u64 n = 0;

	while (isdigit (*s++)) {
		n *= 10;
		n += *(s - 1) - '0';
	}

	return n;
}



u64
div (u64 n, u32 d)
{
	u32 q, r;

	asm (	"divl %[n]\n\t"
		: "=d" (r), "=a" (q)
		: "0" (n >> 32), "1" (n), [n] "rm" (d));

	return q;
}



