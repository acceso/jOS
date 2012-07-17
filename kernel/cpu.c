


#include <stdint.h>

#include <lib/debug.h>
#include <lib/kernel.h>
#include <lib/mem.h>

#include <kernel/intr.h>

#include "cpu.h"




struct _sys sys;

__attribute__((aligned(8))) char stack[STACKSIZE];


static struct _tss {
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
} __attribute__((__packed__,aligned(8))) tss;


static inline void
ltr (u16 sel)
{
	asm volatile (
		"ltr %0" 
		: 
		: "r" (sel));
}


extern struct _gdt tssd;

void
init_cpu ()
{
	u64 tsspa = (u64)__pa (&tss);

	tssd.limit = 0x67;
	/*tssd.limit = sizeof (struct _tss) - 1;*/
	tssd.base1 = (u64)&tsspa & 0xffff;
	tssd.base2 = ((u64)&tsspa >> 16) & 0xff;
	/* busy TSS | present bit */
	tssd.type = (1<<7) | 0b1001;
	tssd.base3 = ((u64)&tsspa >> 24) & 0xff;
	tssd.base4 = ((u64)&tsspa >> 32) & 0xffffffff;


	/* Note: the TSS is  not initialized yet! Should be when needed */

	ltr (0x20);


	return;
}




