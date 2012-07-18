
#ifndef TEST_COMMON_H
#define TEST_COMMON_H


#include <mm/mm.h>
#include <stdarg.h>

#include "test.h"



int vprintf (const char *, va_list);

void
kprintf (const char *fmt, va_list argp)
{
	vprintf(fmt, argp);
}



void
kputs (const char *s)
{
	puts (s);
}



void
kpanic (char *s)
{
	puts (s);
	exit (0);
}




#endif /* TEST_COMMON_H */

