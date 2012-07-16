

#include <stdint.h>
#include <stdio.h>

#include <lib/bitset.h>
#include <lib/debug.h>
#include <lib/kernel.h>

#include "mm.h"
#include "phys.h"
#include "kmalloc.h"



typedef struct multiboot_info {
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
} __attribute__((packed)) multiboot_info_t;



struct multiboot_mmap_entry {
	u32 size;
	u64 addr;
	u64 len;
	u32 type;
} __attribute__((packed));

typedef struct multiboot_mmap_entry multiboot_memory_map_t;



extern u32 mbi32;
static multiboot_info_t *mbi;


struct _usablemem usablemem[MMAP_ARRAY_MAX];



static void
get_memory_ranges_grub (void)
{
	multiboot_memory_map_t *mmap;
	u8 i = 0;


	mmap = (multiboot_memory_map_t *)(u64)mbi->mmap_addr;


	if ((mbi->flags & (1<<0)) == 0x0)
		kpanic ("Can't detect memory!\n");

	kprintf ("%i lower and %i upper KB detected. Usable ranges:\n",
		mbi->mem_lower, mbi->mem_upper);

	/*if ((mbi->flags & (1<<5)) == 0x0)
		kpanic ("Invalid memory map!\n");*/


	/*kprintf ("mmap_addr = %p, mmap_length = %p\n",
		(unsigned) mbi->mmap_addr,
		(unsigned) mbi->mmap_length);*/


	while ((u64)mmap < mbi->mmap_addr + mbi->mmap_length) {
		if (mmap->type == 1) {

			kprintf ("  base: %10p    limit: %10p    "
				 "(%10p B)\t\n", 
				 mmap->addr, mmap->addr + mmap->len,
				 mmap->len);

			usablemem[i].addr = (u64 *)mmap->addr;
			usablemem[i++].len = mmap->len;

		}
		mmap = (multiboot_memory_map_t *)((u64)mmap + mmap->size
			+ sizeof (mmap->size));
	}


	return;
}


#if 0
static void
get_memory_ranges_e820 (void)
{

#define SMAP 0x534D4150

	register u32 ebx asm ("%%ebx") = 0;
	/* Up to this point, no low memory has to be in use
	 * (should be obvious because we're detecting it).
	 * No other function should use these values. */
	register u16  di asm ("%%di") = (u16)(u64) /* We skip 0 */
		align_to ((multiboot_memory_map_t *)1, MWORD); 

	register u32 eax asm ("%%eax");
	register u32 edx asm ("%%edx");
	register u32 ecx asm ("%%ecx");


	do {
		eax = 0xe820;
		edx = SMAP; /* "SMAP" Signature */
		ecx = sizeof (multiboot_memory_map_t) - 4;

		/*call_bios ("int $0x15\n");*/

		if (edx != SMAP) {
			kprintf ("e820 failed! Can do no more :(\n");
			break;
		}

		kprintf ("%d\n", mmap.addr);
		kprintf ("%d\n", mmap.len);
		kprintf ("%d\n", mmap.type);

	} while (ebx != 0);



	return;


}
#endif



void
init_memory (void)
{
	mbi = (multiboot_info_t *)((u64)mbi32);


	/* Use grub's provided memory if available,
	 * otherwise, fallback to e820 bios method. */
	/* Note: the a.out kludge messes up with this (I guess)
	if ((mbi->flags & (1<<5)) != 0x0)
		get_memory_ranges_grub ();
	else
		get_memory_ranges_e820 ();
	*/
	get_memory_ranges_grub ();

	build_page_frames ();
	heap_init ();

}




