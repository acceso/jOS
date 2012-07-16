


#include <stdint.h>

#include <lib/kernel.h>

#include <kernel/intr.h>

#include "cpu.h"




struct _sys sys;

__attribute__((aligned(8))) char stack[STACKSIZE];


static struct {
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



extern struct _gdt *tssd;

void
init_cpu ()
{
	tssd->limit = 0x67;
	tssd->base1 = (u64)&tss & 0xffff;
	tssd->base2 = ((u64)&tss >> 16) & 0xff;
	/* busy TSS | present bit */
	tssd->type = 0xb | (1<<7);
	tssd->base3 = ((u64)&tss >> 24) & 0xff;
	tssd->base4 = ((u64)&tss >> 32) & 0xffffffff;

	tss.rsp0 = (u64)stack;

return;
	asm volatile ("ltr 0x18");
}




