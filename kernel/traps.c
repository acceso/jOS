

#include <inc/boot.h>
#include <inc/types.h>
#include <drivers/vga.h>
#include <lib/stdio.h>

#include "traps.h"


static struct {
	u16 offset1;
	u16 selector;
	u8 ist;
	u8 flags;
	u16 offset2;
	u32 offset3;
	u32 reserved;
} __attribute__((__packed__, aligned(8))) idtentry[256];




static void 
set_idt_reg (u64 base, u16 limit)
{
	struct {
		u16 limit;
		u64 base;
	} __attribute__((__packed__)) idt_reg;

	idt_reg.base = base;
	idt_reg.limit = limit;

	asm volatile("lidt %0"::"m" (idt_reg));
}


__isr__ static void
isr_generic_handler (struct intr_frame r)
{
	intr_enter (0);

	kprintf ("Exception %d!\n", r.intnum);

	intr_exit ();
}



void
idt_set_gate (u8 num, u64 addr, u16 selector, u16 flags)
{
	idtentry[num].offset1 = addr & 0xFFFF;
	idtentry[num].offset2 = (addr >> 16) & 0xFFFF;
	idtentry[num].offset3 = (addr >> 32);
	idtentry[num].selector = selector;
	idtentry[num].flags = flags;

	idtentry[num].reserved = 0;
	idtentry[num].ist = 0;

}



void
init_exceptions (void)
{
	u8 n;

	for (n = 0; n <= 254; n++)
		idt_set_gate (n, (u64)&isr_generic_handler, K_CS, GATE_INT);

	set_idt_reg ((u64) idtentry, sizeof(idtentry) - 1);

}





