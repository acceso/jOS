

#include <stdint.h>
#include <stdio.h>
#include <drivers/vga.h>
#include <lib/kernel.h>

#include "traps.h"
#include "intr.h"



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



__isr__
exc_generic_handler ()
{
	intr_enter ();

	kprintf ("Exception!\n");

	intr_exit ();
}



__isr__
exc_0_divideby0 (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Divide by 0!\n");

	intr_exit ();
}



__isr__
exc_1_debug (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Debug!\n");

	intr_exit ();
}



__isr__
exc_2_nmi (struct intr_frame r)
{
	intr_enter ();

	kprintf ("NMI!\n");

	intr_exit ();
}



__isr__
exc_3_breakpoint (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Breakpoint!\n");

	intr_exit ();
}



__isr__
exc_4_overflow (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Overflow!\n");

	intr_exit ();
}



__isr__
exc_5_bound (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Bound!\n");

	intr_exit ();
}



__isr__
exc_6_iopcode (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Invalid opcode!\n");

	intr_exit ();
}



__isr__
exc_7_nomathco (struct intr_frame r)
{
	intr_enter ();

	kprintf ("No math coprocessor!\n");

	intr_exit ();
}



__isr__
exc_8_doublefault (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("Double fault!\n");

//	kprintf ("\n%llp\n", &r);

#if 0 // Meter esto en una función!
	kprintf ("r11    (%llp)\n", r.r11);
	kprintf ("r10    (%llp)\n", r.r10);
	kprintf ("r9     (%llp)\n", r.r9);
	kprintf ("r8     (%llp)\n", r.r8);
	kprintf ("rdi    (%llp)\n", r.rdi);
	kprintf ("rsi    (%llp)\n", r.rsi);
	kprintf ("rdx    (%llp)\n", r.rdx);
	kprintf ("rcx    (%llp)\n", r.rcx);
	kprintf ("rax    (%llp)\n", r.rax);
	kprintf ("ecode  (%llp)\n", r.ecode);
	kprintf ("retrip (%llp)\n", r.retrip);
	kprintf ("cs     (%llp)\n", r.cs);
	kprintf ("rflags (%llp)\n", r.rflags);
	kprintf ("retrsp (%llp)\n", r.retrsp);
	kprintf ("ss     (%llp)\n", r.ss);
#endif

	intr_exit ();
}



__isr__
exc_10_tss_inval (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("Invalid TSS!\n");

	intr_exit ();
}



__isr__
exc_11_nosuchsegment (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("Segment not present!\n");

	intr_exit ();
}



__isr__
exc_12_stack (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("Stack exception!\n");

	intr_exit ();
}



__isr__
exc_13_gp (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("General protection fault!\n");

	intr_exit ();
}



__isr__
exc_14_pf (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("Page fault!\n");

	intr_exit ();
}



__isr__
exc_16_math_pending (struct intr_frame r)
{
	intr_enter ();

	kprintf ("x87 floating-point exception pending!\n");

	intr_exit ();
}



__isr__
exc_17_misalignment (struct exceptp_frame r)
{
	intr_enter ();

	kprintf ("Misaligned memory access!\n");

	intr_exit ();
}



__isr__
exc_18_machinecheck (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Machine check!\n");

	intr_exit ();
}



__isr__
exc_19_simd (struct intr_frame r)
{
	intr_enter ();

	kprintf ("SIMD exception!\n");

	intr_exit ();
}



__isr__
exc_30_security (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Security exception!\n");

	intr_exit ();
}



__isr__
isr_generic_handler (struct intr_frame r)
{
	intr_enter ();

	/*kprintf ("Interrupt!\n");*/

	lapic_eoi ();
	intr_exit ();
}





static struct {
	u16 offset1;
	u16 selector;
	u8 ist;
	u8 flags;
	u16 offset2;
	u32 offset3;
	u32 reserved;
} __attribute__((__packed__, aligned(8))) idtentry[256];



void
idt_set_gate (u8 num, u64 addr, u16 selector, u16 flags)
{
	/* No, i'm not smoking crack :)
	 * gcc doesn't support "naked" functions, see:
	 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=25967
	 * Until this bug gets fixed, I have come up with this hack:
	 * instead of pointing at the function, I skip several bytes,
	 * and add a padding of "nop" instructions.
	 * It sucks but it's better than any other solution */
	addr += 16;

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

	idt_set_gate (0, (u64)&exc_0_divideby0, K_CS, GATE_INT);
	idt_set_gate (1, (u64)&exc_1_debug, K_CS, GATE_INT);
	idt_set_gate (2, (u64)&exc_2_nmi, K_CS, GATE_INT);
	idt_set_gate (3, (u64)&exc_3_breakpoint, K_CS, GATE_INT);
	idt_set_gate (4, (u64)&exc_4_overflow, K_CS, GATE_INT);
	idt_set_gate (5, (u64)&exc_5_bound, K_CS, GATE_INT);
	idt_set_gate (6, (u64)&exc_6_iopcode, K_CS, GATE_INT);
	idt_set_gate (7, (u64)&exc_7_nomathco, K_CS, GATE_INT);
	idt_set_gate (8, (u64)&exc_8_doublefault, K_CS, GATE_INT);
	idt_set_gate (9, (u64)&exc_generic_handler, K_CS, GATE_INT);
	idt_set_gate (10, (u64)&exc_10_tss_inval, K_CS, GATE_INT);
	idt_set_gate (12, (u64)&exc_11_nosuchsegment, K_CS, GATE_INT);
	idt_set_gate (13, (u64)&exc_12_stack, K_CS, GATE_INT);
	idt_set_gate (14, (u64)&exc_13_gp, K_CS, GATE_INT);
	idt_set_gate (15, (u64)&exc_14_pf, K_CS, GATE_INT);
	idt_set_gate (16, (u64)&exc_generic_handler, K_CS, GATE_INT);
	idt_set_gate (17, (u64)&exc_16_math_pending, K_CS, GATE_INT);
	idt_set_gate (18, (u64)&exc_17_misalignment, K_CS, GATE_INT);
	idt_set_gate (19, (u64)&exc_18_machinecheck, K_CS, GATE_INT);
	idt_set_gate (10, (u64)&exc_19_simd, K_CS, GATE_INT);

	for (n = 20; n <= 29; n++)
		idt_set_gate (n, (u64)&exc_generic_handler, K_CS, GATE_INT);

	idt_set_gate (30, (u64)&exc_30_security, K_CS, GATE_INT);
	idt_set_gate (31, (u64)&exc_generic_handler, K_CS, GATE_INT);



	for (n = 32; n <= 254; n++)
		idt_set_gate (n, (u64)&isr_generic_handler, K_CS, GATE_INT);



	set_idt_reg ((u64) idtentry, sizeof(idtentry) - 1);

}





