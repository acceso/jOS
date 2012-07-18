
#include <mm/kcache.h>

#include "common.h"
#include "mem_common.h"


struct ura {
	int a;
	char s[10];
};



void *
kma_alloc (size_t size)
{
	return malloc (size);
}






void
kma_free (void *addr)
{

}


#define SZ	4057

void *ptrs[SZ];

int
main (int argc, char **argv)
{
	struct kcache *c;
	int i;


	c = kcache_create ("test", sizeof (struct ura));

	for (i = 0; i < SZ; i++)
		ptrs[i] = kcache_alloc (c);

	void *ejem = kcache_alloc (c);
	kcache_free (c, ejem);

	for (i = 0; i < SZ; i++)
		kcache_free (c, ptrs[i]);


	printf ("Nothing :(.");

	kcache_destroy (c);

	return 0;
}




