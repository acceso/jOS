

#include <stdint.h>
#include <stdio.h>

#include <lib/bitset.h>
#include <lib/debug.h>
#include <lib/kernel.h>

#include "mm.h"
#include "phys.h"
#include "kma.h"



/* Grub uses this format for the memory ranges it gets from BIOS */
typedef struct {
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	u32 boot_device;
	u32 cmdline;
	u32 mods_count;
	u32 mods_addr;
	u32 a, b, c, d;
	u32 mmap_length;
	u32 mmap_addr;
	u32 drives_length;
	u32 drives_addr;
	u32 config_table;
	u32 boot_loader_name;
	u32 apm_table;
} __attribute__((packed)) mboot_info;


/* mboot_info->mmap_addr contains a pointer to an array 
 * with this format and mboot_info->mmap_length bytes: */
typedef struct {
	u32 size;
	u64 addr;
	u64 len;
	u32 type;
} __attribute__((packed)) mboot_mmap_info;



/* Set on assembly code,
 * points to the mboot_info structure used by grub in 32 bit cpu mode */
extern u32 mbi32;

/* Our pointer will be 64 bits so we need to extend it. */
static mboot_info *mbi;




/* Get the n range. if (mbi->flags & (0b1 << 1)) not set,
 * we should fallback to e820 bios method.
 * 
 * Note we use this "stateless" function because although it's expensive
 * to loop over the memory map foreach entry, we avoid to reserve memory.
 */
u64 
get_mm_range (void **addr, u16 n)
{
	mboot_mmap_info *mmap;
	u8 i = 0;


	mmap = (mboot_mmap_info *)(u64)mbi->mmap_addr;


	while ((u64)mmap < mbi->mmap_addr + mbi->mmap_length) {
		if (mmap->type == 1 && i++ == n) {
			*addr = (void *)mmap->addr;
			return mmap->len;
		}

		mmap = (mboot_mmap_info *)((u64)mmap + mmap->size
			+ sizeof (mmap->size));
	}

	return 0;
}



void
init_memory (void)
{
	mbi = (mboot_info *)((u64)mbi32);

	kprintf ("%i lower and %i upper KB detected.\n",
		mbi->mem_lower, mbi->mem_upper);

	if ((mbi->flags & (1<<0)) == 0x0)
		kpanic ("Can't detect memory!\n");

	/* FIXME: as I understand the spec,
	 * the sixth bit should be 1 but it's 0... */
	/* if ((mbi->flags & (1<<5)) == 0x0)
		kpanic ("Invalid memory map!\n"); */


	build_page_frames ();
	kma_init ();

}




