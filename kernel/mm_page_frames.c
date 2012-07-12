

#include <inc/types.h>


extern u64 bss2;

static struct _page_frame {
	u64 flags;
	u64 priv;
} *page_frame;


/*list_head*/void *free_blocks[20];


u32 npages;


void
build_page_frames (void)
{
	struct _page_frame *p;
	u64 i;

	page_frame = (struct _page_frame *)&bss2;
	p = page_frame;


	/* TODO: this should be use the memory map information... for now, i'll use
	 * from the 4th mega up to 128 MB (0x400000 - 0x8000000 or 64 x 2MB-pages).
	 */
	for (i = 0; i < 64; i++) {
		npages++;
		(page_frame + i)->flags = 0;
		(page_frame + i)->priv = 0;
	}

	/* it gets easier when you know the amount of memory available :D :D */
	free_blocks[6] = (void *)0x400000;

	kprintf ("%d page frames initialized!\n", npages);
}



