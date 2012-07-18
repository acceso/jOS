

#include <mm/kma.h>

#include "common.h"
#include "mem_common.h"


#define KB	1024
#define MB	(1024*1024)




void
init_slabs (void)
{
}


void
init_kmalloc (void)
{
}



int
main (int argc, char **argv)
{
	void *a, *b, *c, *d;

	kma_init ();

	a = _kma_alloc (17*KB);
	b = _kma_alloc (34*KB);
	c = _kma_alloc (2*MB);
	d = _kma_alloc (1*MB);

	_kma_free (d);
	_kma_free (a);
	_kma_free (c);
	_kma_free (b);

	a = _kma_alloc (17*KB);
	_kma_free (a);

	return 0;
}



