

#include <stdint.h>
#include <stdio.h>

#include <lib/bitset.h>
#include <lib/kernel.h>
#include <lib/mem.h>

#include "mm.h"



/* These have to be used by taking the address, see:
 * http://sourceware.org/binutils/docs/ld/Source-Code-Reference.html 
 * Remember: sok is physical but eok is virtual memory! */
extern u64 sok, eok;



/* Max physical mem:
 * MEM_BITMAP_MAX entries * 64 bits each * page size bytes, or:
 * 32 * 64 * pow (2,21) */
#define MEM_BITMAP_MAX 32

static struct {
	/* 1 for busy and 0 for free */
	u64 memmap[MEM_BITMAP_MAX];
	u64 npages;
} zone;



/* Returns the address of the start of a given page frame number. */
static void *
pfn_to_addr (u64 pfn)
{
	return (void *)(pfn * PAGE_SIZE);
}



/* Returns the page frame number corresponding to a given address. */
static inline u64
addr_to_pfn (void *addr)
{
	return (u64)addr >> PAGE_OFFSET;
}





/* Marks as free the page that contains addr */
static void
pf_mark_free_addr (void *addr)
{
	u64 idx = addr_to_pfn (addr) >> 6;

	if (idx > MEM_BITMAP_MAX)
		return;

	/* To understand this code:
	 * idx = page_frame_number / 64;
	 * bit = page_frame_number % 64; */
	bitclear (&zone.memmap[idx], (u64)addr_to_pfn (addr) & 0b111111);
}



static s8 
mem_usable (void *addr)
{
	/* Within kernel code */
	if (addr + PAGE_SIZE >= (void *)&sok &&
		addr < (void *)__pa ((void *)&eok))
			return 0;

	/* Within lapic, io apic, bios... addresses */
	if (addr + PAGE_SIZE >= (void *)0xfec00000 &&
		addr < (void *)0xffffffff)
			return 0;


	return 1;
}


/* Gets the ith entry from the memory map,
 * and marks the page frames contained within (if any) as available.
 * Returns the number of pages made available. */
static s64
mm_make_range_usable (void *addr, u64 len)
{
	void *p;
	u64 npages = 0;


	/* This segment is not enough for even a page. */
	if (len < PAGE_SIZE)
		return 0;

	p = align_to (addr, PAGE_SIZE);

	do {
		/* No more pages */
		if (p + PAGE_SIZE > addr + len)
			return npages; 

		if (mem_usable (p)) {
			pf_mark_free_addr (p);
			npages++;
		}
	
		p += PAGE_SIZE;
	} while (p <= addr + len);


	return npages;
}



void
build_page_frames (void)
{
	u8 i;
	void *addr = 0;
	u64 len;


	/* Initially, all memory is unusable (every bit set to 1). */
	for (i = 0; i < MEM_BITMAP_MAX; i++)
		zone.memmap[i] = ~0L;


	i = 0;

	do {
		len = get_mm_range (&addr, i++);
		if (len == 0)
			break;

		zone.npages += mm_make_range_usable (addr, len);
	} while (1);


	kprintf ("  %d free page frames\n", zone.npages);

}






/************************************************************************/





static s32
find_free_pages (u32 order)
{
	u64 current_word;
	u64 i = 0;
	s32 bit;

	do {
		/* Small optimization: this one is full */
		if (zone.memmap[i] == ~0L)
			continue;

		bit = 0;
		current_word = zone.memmap[i];

		do {
			if ((current_word & 0x1) == 0)
				break;

			bit++; 
			current_word >>= 1;
		} while (bit < MACHINEBITS);

		if (bit < MACHINEBITS) {
			bitset (&zone.memmap[i], bit);
			return bit;
		}


	} while (++i < MEM_BITMAP_MAX);


	return -1;
}





void *
get_pages (u32 order)
{
	s32 pfn;

	/* TODO: just one page for now */
	if (order > 0)
		kpanic ("Not yet implemented!\n");

	pfn = find_free_pages (order);
	if (pfn < 0)
		kpanic ("Out of memory!\n");

	return (void *)__va (pfn_to_addr (pfn));
}



void *
get_one_page (void)
{
	return get_pages (0);
}



void
free_page (u64 *addr)
{
	/* Not needed, pf_mark_free_addr can be called twice
	if (pfn_isfree (addr_to_pfn (addr)))
		return; */

	pf_mark_free_addr (addr);
}



void
free_page_pfn (u64 pfn)
{
	pf_mark_free_addr ((u64 *)pfn_to_addr (pfn));
}


