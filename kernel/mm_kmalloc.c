

#include <inc/types.h>
#include <lib/list.h>
#include <lib/kernel.h>
#include <lib/bitset.h>
#include <lib/stdio.h>
#include "mm_phys.h"
#include "mm.h"

static const u32 blocksizes [] = {
	32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144
};


struct _mblock {
	u64 addr;
	struct list_head list;
};

struct _mpages {
	struct list_head pages;
	u64 avl_mem;
	u64 free_mem;
	struct _mblock *holes[sizeof (blocksizes) / sizeof (u32)];
	struct _mblock *blocks[sizeof (blocksizes) / sizeof (u32)];
};

static struct {
	struct _mpages *pages;
	u64 npages;
	u64 avl_mem;
	u64 free_mem;
} mpool;


static void
pool_one_page_init (struct _mpages *mp)
{
	u64 *p;
	s64 i;

	p = (u64 *)((u64)mp + sizeof (struct _mpages));

	mp->avl_mem = mp->free_mem = PAGE_SIZE - sizeof (struct _mpages);

	p = align_to (p, MWORD);

	i = sizeof (blocksizes) / sizeof (u32);

	while (--i >= 0) {
		while ((u64)p + blocksizes[i] < (u64)mp + PAGE_SIZE) {

			((struct _mblock *)p)->addr = (u64)p + sizeof (struct _mblock);

			if (mp->holes[i] == NULL)
				mp->holes[i] = (struct _mblock *)p;
			else
				list_add (&((struct _mblock *)p)->list, &mp->holes[i]->list);

			p = (u64 *)((u64)p + blocksizes[i]);
		}
	}


}



static void
pool_add_page (void *addr)
{
	struct _mpages *mp = (struct _mpages *) addr;

	pool_one_page_init (mp);

	mpool.free_mem += PAGE_SIZE;
	mpool.avl_mem += PAGE_SIZE;
	mpool.npages++;

	/* Is this is the first allocation? */
	if (mpool.pages == 0x0) {
		list_init (&mp->pages);
		mpool.pages = mp;
	} else
		list_add (&mp->pages, &mpool.pages->pages);

	return;
}


void *
kmalloc (u64 size)
{
	return 0x0;
}



void
kfree (void *addr)
{
	return;
}


void
heap_init (void)
{
	u64 *p;
	
	/* At least one page will be needed.. */
	p = (u64 *) get_one_page ();
	if (p == NULL)
		kpanic ("Out of memory!\n");

	pool_add_page (p);

	return;
}


