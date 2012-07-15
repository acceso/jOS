

#include <stdint.h>
#include <stdio.h>
#include <lib/bitset.h>
#include <lib/kernel.h>
#include <lib/list.h>
#include <lib/mem.h>

#include "mm.h"
#include "phys.h"
#include "kmalloc.h"



static const u32 blocksizes [] = {
	32, 64, 128, 256, 512, 1024, 2048, 4096,
	8192, 16384, 32768, 65536, 131072, 262144
};


struct _mblock {
	u64 addr;
	struct list_head list;
};


struct _mpages {
	struct list_head pages;
	u64 avl_mem;
	u64 free_mem;
	struct list_head holes[sizeof (blocksizes) / sizeof (u32)];
	struct list_head blocks[sizeof (blocksizes) / sizeof (u32)];
	u16 nholes;
	u16 nblocks;
};


static struct {
	struct list_head pages;
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
		list_init (&mp->holes[i]);
		list_init (&mp->blocks[i]);

		while ((u64)p + blocksizes[i] < (u64)mp + PAGE_SIZE) {

			((struct _mblock *)p)->addr = (u64)p
				+ sizeof (struct _mblock);

			list_add_back (&((struct _mblock *)p)->list,
				&mp->holes[i]);

			mp->nholes++;

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

	list_add_back (&mp->pages, &mpool.pages);


	return;
}



/* classical kmalloc, ¿first best fit? */
void *
kmalloc (u64 size, u16 flags)
{
	struct _mpages *mp;
	struct _mblock *mb;
	u8 i;


	/* TODO: not the best algorithm ;) */
	list_foreach_entry (mp, &mpool.pages, pages) {
		i = 0;

		do {
			/* continue if no free blocks of this size
			 * or doesn't fit */
			if (mp->nholes <= 0 || size > blocksizes[i]
					- sizeof (struct _mblock))
				continue;

			mb = containerof (list_pop_del (&mp->holes[i]),
				struct _mblock, list);

			list_add_back (&mb->list, &mp->blocks[i]);


			return (void *)mb->addr;

		} while (i++ < sizeof (blocksizes) / sizeof (u32));
	}

	return (void *)0;
}



void
kfree (void *addr)
{
	/* TODO: who needs to free memory anyway? :D */
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

	list_init (&mpool.pages);

	pool_add_page (p);


	return;
}


