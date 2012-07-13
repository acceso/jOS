

#include <inc/types.h>
#include <lib/stdio.h>
#include <lib/list.h>
#include <kernel/mm.h>
#include <kernel/mm_kmalloc.h>
#include <lib/kernel.h>
#include <lib/bitset.h>



/* These have to be used taking the address, see:
   http://sourceware.org/binutils/docs/ld/Source-Code-Reference.html */
extern u64 sok, eok;


extern struct _usablemem usablemem[MMAP_ARRAY_MAX];


#define MEM_BITMAP_MAX 32 /* Max physical memory: 32 * pow (2,21) * 64 */


static struct _zone {
	u64 memmap[MEM_BITMAP_MAX];
	u64 npages;
} zone;



static u64 *
pfn_to_addr (u64 pfn)
{
	return (u64 *) (pfn * PAGE_SIZE);
}



static inline u64
addr_to_pfn (u64 *addr)
{
	return (u64)addr >> PAGE_OFFSET;
}



static void
pf_mark_free_addr (u64 *addr)
{
	u64 idx = addr_to_pfn (addr) >> 6;

	if (idx > MEM_BITMAP_MAX)
		return;

	/* The key to understand this code is: 2^6 = 64 (aka: log2(64)) and: 0x3f = bin(64-1) = 0b111111 */
	bitclear (&zone.memmap[idx], (u64)addr_to_pfn(addr) & 0x3f);
}



static u64
make_usable (struct _usablemem *r)
{
	/* Point to the start of the next page frame: */
	u64 *base, *p;
	u64 npages = 0;
u32 i = 0;

	/* Advance (if needed) to the next fully free page frame */
	if (PAGE_ALIGNED (r->addr))
		base = r->addr;
	else
		base = (u64 *)(((u64)r->addr & PAGE_ALIGN) + PAGE_SIZE);

	if ((u64)base + PAGE_SIZE > (u64)r->addr + r->len)
		return npages;

	p = base;

	do {
		/* No more pages */
		if ((u64)p + PAGE_SIZE > (u64)r->addr + r->len)
			return npages; 

		/* Don't mess up with kernel code! */
		if ((u64)p + PAGE_SIZE > (u64)&sok && p <  __pa (&eok)) {
			p = (u64 *)((u64)p + PAGE_SIZE);
			continue;
		}

		pf_mark_free_addr (p);
		npages++;

		p = (u64 *)((u64)p + PAGE_SIZE);
	} while ((u64)p <= (u64)r->addr + r->len);


	return npages;
}



void
build_page_frames (void)
{
	u8 i;

	/* Initially, all memory is unusable. */
	for (i = 0; i < MEM_BITMAP_MAX; i++)
		zone.memmap[i] = ~0L;

	i = 0;

	do {
		zone.npages += make_usable (&usablemem[i]);
	} while (i < MMAP_ARRAY_MAX && usablemem[++i].len != 0);


	kprintf ("%d page frames initialized!\n", zone.npages);
}



static s32
find_free_pages (u32 order)
{
	u64 current_word;
	u64 i = 0;
	u8 bit;

	do {
		/* Small optimization: this one is full */
		if (zone.memmap[i] == ~0L)
			continue;

		bit = 0;
		current_word = zone.memmap[i];

		do {
			if ((current_word & 0x1) == 0)
				break;

			current_word >>= 1;
			bit++; 
		} while (bit < MWORD * 8);

		if (bit < MWORD * 8)
			return bit;

	} while (++i < MEM_BITMAP_MAX);

	return -1;
}



u64 *
get_pages (u32 order)
{
	s32 pfn;

	/* TODO: just can ask for one page for now */
	if (order > 0)
		kpanic ("Not yet implemented!\n");

	pfn = find_free_pages (order);
	if (pfn < 0)
		kpanic ("Out of memory!\n");

	return __va(pfn_to_addr (pfn));
 
}


u64 *
get_page (void)
{
	return get_pages (0);
}



