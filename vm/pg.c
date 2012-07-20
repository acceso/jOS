

#include <stdint.h>

#include <lib/debug.h>
#include <lib/kernel.h>
#include <lib/mem.h>
#include <mm/kmalloc.h>

#include <kernel/task.h>

#include "pg.h"




static void
cr3_load (void *pml4)
{
	if (((u64)pml4 & 0x7ff8000000000fffL) != 0)
		kpanic ("Invalid cr3 location!\n");

	cr3_write ((u64)pml4);
}



static void *
pte_set (void *src, void *dst, u8 user_readable)
{
	/* TODO: use writeback: 0b1000 */
	*(u64 *)src = (u64)__pa (dst) + (user_readable << 2) + 0b11;

	return dst;
}



static void *
pte_follow (void *addr)
{
	return __va (*(u64 *)addr & ~0xfff8000000000fffL);
}



static void *
pte_get (void)
{
	return xkmalloc0 (4096);
}



void
load_init_page_tables (void)
{
	u16 i;
	u64 *p;
	u64 *pa;
	u64 *pd;

	current->mm = xkmalloc0 (sizeof (struct mm));

	init_mm (current->mm, pte_get ());


	p = pte_set (current->mm->pgd + 256, pte_get (), 0);

/* For 1GB page tables: */
//	for (i = 0; i < 512; i++)
//		*(p + i) = i * (2<<29) + 0b110000111L;

	pa = pte_set (p + 0, pte_get (), 0);
	pd = pte_set (p + 3, pte_get (), 0);

	for (i = 0; i < 512; i++) {
		*(pa + i) = 0x87 + i * 0x200000;
		*(pd + i) = 0xc0000087 + i * 0x200000;
	}



	cr3_load (__pa (current->mm->pgd));
}



