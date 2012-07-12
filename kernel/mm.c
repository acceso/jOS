

#include <inc/types.h>

#include <lib/stdio.h>

#include "mm.h"
#include "mm_page_frames.h"


extern u32 mbi32;


static void
get_memory_ranges (void)
{
	multiboot_info64_t *mbi = (multiboot_info64_t *)((u64)mbi32);
	multiboot_memory_map64_t *mmap = (multiboot_memory_map64_t *)((u64) mbi->mmap_addr);

	kprintf ("Memory detected: %i KB\n", mbi->mem_upper);

	return;

	while (mmap < (multiboot_memory_map64_t *)((u64)mbi->mmap_addr) + mbi->mmap_length) {
		kprintf("Base address: %x - Size: %x\n - Type: %d", mmap->base_addr, mmap->length, mmap->type);

		mmap += mmap->size + sizeof(u32);
	}
}



void
init_memory (void)
{
	get_memory_ranges ();
	build_page_frames ();

}




