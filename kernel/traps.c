

#include <inc/boot.h>
#include <inc/types.h>
#include <drivers/vga.h>
#include <lib/stdio.h>

#include "traps.h"
#include "intr.h"


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
exc_generic_handler (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Exception!\n");

	intr_exit ();
}


__isr__ static void
exc_0_divideby0 (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Divide by 0!\n");

	intr_exit ();
}


__isr__ static void
exc_1_debug (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Debug!\n");

	intr_exit ();
}


__isr__ static void
exc_2_nmi (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("NMI!\n");

	intr_exit ();
}


__isr__ static void
exc_3_breakpoint (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Breakpoint!\n");

	intr_exit ();
}


__isr__ static void
exc_4_overflow (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Overflow!\n");

	intr_exit ();
}


__isr__ static void
exc_5_bound (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Bound!\n");

	intr_exit ();
}


__isr__ static void
exc_6_iopcode (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Invalid opcode!\n");

	intr_exit ();
}


__isr__ static void
exc_7_nomathco (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("No math coprocessor!\n");

	intr_exit ();
}


__isr__ static void
exc_8_doublefault (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Double fault!\n");

	intr_exit ();
}


__isr__ static void
exc_10_tss_inval (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Invalid TSS!\n");

	intr_exit ();
}


__isr__ static void
exc_11_nosuchsegment (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Segment not present!\n");

	intr_exit ();
}


__isr__ static void
exc_12_stack (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Stack exception!\n");

	intr_exit ();
}


__isr__ static void
exc_13_gp (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("General protection fault!\n");

	intr_exit ();
}


__isr__ static void
exc_14_pf (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Page fault!\n");

	intr_exit ();
}


__isr__ static void
exc_16_math_pending (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("x87 floating-point exception pending!\n");

	intr_exit ();
}


__isr__ static void
exc_17_misalignment (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Misaligned memory access!\n");

	intr_exit ();
}


__isr__ static void
exc_18_machinecheck (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Machine check!\n");

	intr_exit ();
}


__isr__ static void
exc_19_simd (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("SIMD exception!\n");

	intr_exit ();
}


__isr__ static void
exc_30_security (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Security exception!\n");

	intr_exit ();
}



__isr__ static void
isr_generic_handler (struct intr_frame r __attribute__ ((unused)))
{
	intr_enter ();

	kprintf ("Interrupt!\n");

	lapic_eoi ();
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





