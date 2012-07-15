

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



