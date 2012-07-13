

#include <inc/types.h>
#include <lib/list.h>
#include "mm_phys.h"


static struct {
	

	struct list_head pages;
} reserved_pages;



void *
kmalloc (u64 size)
{
	return 0x0;
}



void
kfree (void *addr)
{
}



