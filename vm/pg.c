

#include <stdint.h>

#include <lib/debug.h>
#include <lib/kernel.h>
#include <lib/mem.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>

#include <kernel/task.h>

#include "pg.h"




static void cr3_load(void *pml4)
{
	if (((u64)pml4 & 0x7ff8000000000fffL) != 0)
		kpanic("Invalid cr3 location!\n");

	cr3_write((u64)pml4);
}



/* TODO: right now, I'm using this function for pml4, pdpe and pde entries.
 * I should make 3 different functions or at least 2 (pml4 and pdpe are similar) */
static void *pte_set(void *src, void *dst, u8 user_readable, u8 last)
{
	/* TODO: use writeback: 0b1000 */
	*(u64 *)src = (u64)__pa (dst)
		| (last << 7)
		| (user_readable << 2) /* User / Supervisor */
		| 0b10 /* R / W */
		| 0b1; /* Present */

	return dst;
}



static void *pte_get_empty(void)
{
	return xkmalloc0(4096);
}





static void *pml4_walk(const void *addr)
{
	return pte_addr(current->mm->pgd + pml4_offset(addr));
}



static void *pdpe_walk(const u64 *pdpe, const void *addr)
{
	return pte_addr(pdpe + pdpe_offset(addr));
}



static void *pde_walk(const u64 *pde, const void *addr)
{
	return pte_addr(pde + pde_offset(addr)) + ((u64)addr & 0xfffff);
}



static u8 is_canonical(const void *addr)
{
	if ((u64)addr >> 47 == 0 || (u64)addr >> 47 == 0x1ffff)
		return 1;

	return 0;
}



void *virt2phys(const void *vaddr)
{
	u64 *pdpe;
	u64 *pde;

	if (!is_canonical(vaddr))
		return NULL;

	pdpe = pml4_walk(vaddr);
	if (pdpe == NULL)
		return NULL;

	/* Note: pdpe will point to a physical address
	 * but the virtual one is needed to reach the next level. */
	pde = pdpe_walk(__va(pdpe), vaddr);
	if (pde == NULL)
		return NULL;

	return pde_walk(__va(pde), vaddr);
}



/* TODO: the user readable bit should probably be settable... */
u8 map_one_page(void *phys, void *vaddr)
{
	u64 *pdpe;
	u64 *pde;

	if (!is_canonical(vaddr) || !is_canonical(phys))
		return 0;

	pdpe = pml4_walk(vaddr);
	if (pdpe == NULL)
		pdpe = pte_set(current->mm->pgd + pml4_offset(vaddr), pte_get_empty(), 1, 0);

	pde = pdpe_walk(pdpe, vaddr);
	if (pde == NULL)
		pde = pte_set(pdpe + pdpe_offset(vaddr), pte_get_empty(), 1, 0);

	/* __va() is needed because pte_set gets a kernel mapped va.
	 * PAGE_ALIGN should not be needed, but let's be safe. */
	pte_set(pde + pde_offset(vaddr), __va(phys & PAGE_ALIGN), 1, 1);

	return 1;
}



/* It makes up the initial page tables.
 * The next ones will be cloned. */
void load_init_page_tables(struct task *task)
{
	u16 i;
	u64 *p;
	u64 *pa;
	u64 *pd;

	task->mm = xkmalloc0(sizeof(struct mm));

	init_mm(task->mm, pte_get_empty());


	p = pte_set(task->mm->pgd + 256, pte_get_empty(), 0, 0);

	/* For 1GB page tables: */
	// for (i = 0; i < 512; i++)
	//	*(p + i) = i * (2 << 29) + 0b110000111L;

	pa = pte_set(p + 0, pte_get_empty(), 0, 0);
	pd = pte_set(p + 3, pte_get_empty(), 0, 0);

	for (i = 0; i < 512; i++) {
		*(pa + i) = 0x87 + i * 0x200000;
		*(pd + i) = 0xc0000087 + i * 0x200000;
	}



	cr3_load(__pa(task->mm->pgd));
}




