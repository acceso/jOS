

#include <inc/types.h>
#include <lib/stdio.h>
#include <lib/list.h>




extern u64 bss2;

/* One for each page frame */
struct _page_frame {
	u32 flags;
};


struct _free_blocks {
	union {
		u64 *addr;
		/* ok, the first element will be used as a header so we can place
		 * something here if needed */
		u64 whatever;
	};
	struct list_head list;
};

#define FREE_BLOCKS_MAX_POWER 20

static struct _zone {
	struct _page_frame *page_frame;
	struct _free_blocks free_blocks[FREE_BLOCKS_MAX_POWER];
	u32 npages_total;
	u32 npages_free;
} zone;




void
build_page_frames (void)
{
	u64 i;

	zone.page_frame = (struct _page_frame *)&bss2;


	/* TODO: this should be use the memory map information... for now, i'll use
	 * from the 4th mega up to 128 MB (0x400000 - 0x8000000 or 64 x 2MB-pages).
	 */
	zone.npages_total = 64;
	zone.npages_free = 64;

	for (i = 0; i < 64; i++)
		zone.page_frame[i].flags = 0;

/*
	for (i = 0; i < FREE_BLOCKS_MAX_POWER; i++) {
		free_blocks[i].addr = (u64 *)0;
		list_init (&free_blocks[i].list);
	}
*/
	/* it gets easier when you know the amount of memory available :D :D 
	 * TODO: use the available memory... */
/*	free_blocks[6].addr = (void *)0x400000;
*/
	kprintf ("%d page frames initialized!\n", zone.npages_total);
}



