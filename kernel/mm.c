

#include <inc/types.h>

#include <lib/stdio.h>
#include <lib/kernel.h>

#include "mm.h"
#include "mm_phys.h"



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
} multiboot_info_t;


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
get_memory_ranges (void)
{
	multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)(u64)mbi->mmap_addr;
	u8 i = 0;

	if ((mbi->flags & (1<<0)) == 0x0)
		kpanic ("Can't detect memory!\n");

	kprintf ("%i lower and %i upper KB detected. Usable ranges:\n",
		mbi->mem_lower, mbi->mem_upper);

	if ((mbi->flags & (1<<5)) == 0x0)
		kpanic ("Invalid memory map!\n");


	/*kprintf ("mmap_addr = %p, mmap_length = %p\n",
		(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);*/


	while ((u64)mmap < mbi->mmap_addr + mbi->mmap_length) {
		if (mmap->type == 1) {

			kprintf ("    base: %p     limit: %p\t(%p B)\t\n", 
				mmap->addr, mmap->addr + mmap->len, mmap->len);

			usablemem[i].addr = (u64 *)mmap->addr;
			usablemem[i++].len = mmap->len;

		}
		mmap = (multiboot_memory_map_t *)((u64)mmap + mmap->size + sizeof (mmap->size));
	}


	return;
}



void
init_memory (void)
{
	mbi = (multiboot_info_t *)((u64)mbi32);

	get_memory_ranges ();
	build_page_frames ();

}




