


#include <stdint.h>

#include <lib/debug.h>
#include <lib/kernel.h>
#include <lib/mem.h>

#include <kernel/intr.h>

#include "cpu.h"




struct sys sys;


__attribute__((aligned (8))) char stack[STACKSIZE];



#define GDT_NENTRIES	10

u64 gdt[GDT_NENTRIES] __attribute__ ((aligned (16))) = {
	0x0000000000000000, /* unused */
	0x0000000000000000, /* unused */
	0x0000000000000000, /* unused */
	0x0000000000000000, /* unused */
	0x00af98000000ffff, /* k_cs */
	0x008f92000000ffff, /* k_ds */
	0x002ffa000000ffff, /* u_cs */
	0x008ff2000000ffff, /* u_ds */
	0x0000000000000000, /* tssd */
	0x0000000000000000  /* tssd */
};



struct gdt64_ptr {
	u16 len;
	void *base;
} __attribute__ ((__packed__, aligned (8)));



void
lgdtr (void *gdt, u32 len)
{
	struct gdt64_ptr gdt64_ptr;

	gdt64_ptr.base = gdt;
	gdt64_ptr.len = len;

	asm volatile ("lgdtq %0\n"
		: : "m" (gdt64_ptr));
}






struct tss {
	u32 reserved1;
	u64 rsp0;
	u64 rsp1;
	u64 rsp2;
	u64 reserved2;
	u64 ist1;
	u64 ist2;
	u64 ist3;
	u64 ist4;
	u64 ist5;
	u64 ist6;
	u64 ist7;
	u64 reserved3;
	u16 reserved;
	u16 iomapbase;
} __attribute__ ((__packed__,aligned (8)));



static void
tss_start (void)
{
#if 0
	u64 tsspa;
	struct gdt *tssd;

	tssd = gdt[...];
	
	tsspa = (u64)__pa (&tss);

	tssd.limit = 0x67;
	/*tssd.limit = sizeof (struct tss) - 1;*/
	tssd.base1 = (u64)&tsspa & 0xffff;
	tssd.base2 = ((u64)&tsspa >> 16) & 0xff;
	/* busy TSS | present bit */
	tssd.type = (1<<7) | 0b1001;
	tssd.base3 = ((u64)&tsspa >> 24) & 0xff;
	tssd.base4 = ((u64)&tsspa >> 32) & 0xffffffff;


	/* Note: the TSS is  not initialized yet! Should be when needed */

	ltr (0x20);
#endif
	return;
}



void
init_cpu ()
{
	/* Switch to 64 bit GDTR */
	lgdtr (gdt, 6 * GDT_NENTRIES - 1);

	tss_start ();

	return;
}




