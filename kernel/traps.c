

#include <stdint.h>
#include <stdio.h>

#include <lib/debug.h>
#include <lib/kernel.h>
#include <lib/mem.h>

#include <drivers/vga.h>

#include "traps.h"
#include "intr.h"




static void set_idt_reg(void *base, u16 limit)
{
	struct {
		u16 limit;
		void *base;
	} __attribute__((__packed__)) idt_reg;

	idt_reg.base = base;
	idt_reg.limit = limit;

	asm volatile("lidt %0" ::"m"(idt_reg));
}



__isr__ exc_generic_handler(struct intr_frame r)
{
	intr_enter();

	kprintf("Exception!\n");

	intr_exit();
}



__isr__ exc_0_divideby0(struct intr_frame r)
{
	intr_enter();

	kprintf("Divide by 0!\n");

	intr_exit();
}



__isr__ exc_1_debug(struct intr_frame r)
{
	intr_enter();

	kprintf("Debug!\n");

	intr_exit();
}



__isr__ exc_2_nmi(struct intr_frame r)
{
	intr_enter();

	kprintf("NMI!\n");

	intr_exit();
}



__isr__ exc_3_breakpoint(struct intr_frame r)
{
	intr_enter();

	kprintf("Breakpoint!\n");

	intr_exit();
}



__isr__ exc_4_overflow(struct intr_frame r)
{
	intr_enter();

	kprintf("Overflow!\n");

	intr_exit();
}



__isr__ exc_5_bound(struct intr_frame r)
{
	intr_enter();

	kprintf("Bound!\n");

	intr_exit();
}



__isr__ exc_6_iopcode(struct intr_frame r)
{
	intr_enter();

	kprintf("Invalid opcode!\n");

	intr_exit();
}



__isr__ exc_7_nomathco(struct intr_frame r)
{
	intr_enter();

	kprintf("No math coprocessor!\n");

	intr_exit();
}



__isr__ exc_8_doublefault(struct intr_frame r)
{
	intr_err_enter();

	kprintf("Double fault! (%llp)\n", r.ecode);

	intr_err_exit();
}



__isr__ exc_10_tss_inval(struct intr_frame r)
{
	intr_err_enter();

	kprintf("Invalid TSS!\n");

	intr_err_exit();
}



__isr__ exc_11_nosuchsegment(struct intr_frame r)
{
	intr_err_enter();

	kprintf("Segment not present!\n");

	intr_err_exit();
}



__isr__ exc_12_stack(struct intr_frame r)
{
	intr_err_enter();

	kprintf("Stack exception, segment selector: 0x%lx\n", r.ecode);

	intr_err_exit();
}



__isr__ exc_13_gp(struct intr_frame r)
{
	intr_err_enter();

	kprintf("General protection fault!\n");

	intr_err_exit();
}



__isr__ exc_14_pf(struct intr_frame r)
{
	intr_err_enter();

	kprintf("Page fault!\n");

	intr_err_exit();
}



__isr__ exc_16_math_pending(struct intr_frame r)
{
	intr_enter();

	kprintf("x87 floating-point exception pending!\n");

	intr_exit();
}



__isr__ exc_17_misalignment(struct intr_frame r)
{
	intr_err_enter();

	kprintf("Misaligned memory access!\n");

	intr_err_exit();
}



__isr__ exc_18_machinecheck(struct intr_frame r)
{
	intr_enter();

	kprintf("Machine check!\n");

	intr_exit();
}



__isr__ exc_19_simd(struct intr_frame r)
{
	intr_enter();

	kprintf("SIMD exception!\n");

	intr_exit();
}



__isr__ exc_30_security(struct intr_frame r)
{
	intr_enter();

	kprintf("Security exception!\n");

	intr_exit();
}



__isr__ isr_generic_handler(struct intr_frame r)
{
	intr_enter();

	/*kprintf("Interrupt!\n");*/

	lapic_eoi();
	intr_exit();
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



static void idt_set_gate(u8 num, void *addr, u16 selector, u16 flags)
{
	/* No, i'm not smoking crack :)
	 * gcc doesn't support "naked" functions, see:
	 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=25967
	 * Until this bug gets fixed, I have come up with this hack:
	 * instead of pointing at the function, I skip several bytes,
	 * and add a padding of "nop" instructions.
	 * It sucks but it's better than any other solution */
	addr += 16;

	idtentry[num].offset1 = (u64)addr & 0xffff;
	idtentry[num].offset2 = ((u64)addr >> 16) & 0xffff;
	idtentry[num].offset3 = ((u64)addr >> 32);
	idtentry[num].selector = selector;
	idtentry[num].flags = flags;

	idtentry[num].reserved = 0;
	idtentry[num].ist = 0;

}


extern struct _ioapic ioapic;

/* Note: this is only for interrupts */
void intr_install_handler(u8 num, void *addr)
{
	/* This stuff or redirections is a mess :( */

	switch (num) {
	case 0 ... 31: /* Stuff connected to the io-apic: */
		/* kprintf("->install handler from %d to %d\n",
			num, ioapic.pic[num].dest); */

		idt_set_gate(ioapic.pic[num].dest + 32, addr, K_CS, GATE_INT);
		ioapic_redir_unmask(num);

		break;
	case 32 ... 63: /* IOAPIC interrupts will be moved here */

		break;
	case 64 ... 255: /* Available */
		idt_set_gate(num, addr, K_CS, GATE_INT);
		break;

	}

}



void init_exceptions(void)
{
	u8 n;

	idt_set_gate(0, exc_0_divideby0, K_CS, GATE_INT);
	idt_set_gate(1, exc_1_debug, K_CS, GATE_INT);
	idt_set_gate(2, exc_2_nmi, K_CS, GATE_INT);
	idt_set_gate(3, exc_3_breakpoint, K_CS, GATE_INT);
	idt_set_gate(4, exc_4_overflow, K_CS, GATE_INT);
	idt_set_gate(5, exc_5_bound, K_CS, GATE_INT);
	idt_set_gate(6, exc_6_iopcode, K_CS, GATE_INT);
	idt_set_gate(7, exc_7_nomathco, K_CS, GATE_INT);
	idt_set_gate(8, exc_8_doublefault, K_CS, GATE_INT);
	idt_set_gate(9, exc_generic_handler, K_CS, GATE_INT);
	idt_set_gate(10, exc_10_tss_inval, K_CS, GATE_INT);
	idt_set_gate(12, exc_11_nosuchsegment, K_CS, GATE_INT);
	idt_set_gate(13, exc_12_stack, K_CS, GATE_INT);
	idt_set_gate(14, exc_13_gp, K_CS, GATE_INT);
	idt_set_gate(15, exc_14_pf, K_CS, GATE_INT);
	idt_set_gate(16, exc_generic_handler, K_CS, GATE_INT);
	idt_set_gate(17, exc_16_math_pending, K_CS, GATE_INT);
	idt_set_gate(18, exc_17_misalignment, K_CS, GATE_INT);
	idt_set_gate(19, exc_18_machinecheck, K_CS, GATE_INT);
	idt_set_gate(10, exc_19_simd, K_CS, GATE_INT);

	for (n = 20; n <= 29; n++)
		idt_set_gate(n, exc_generic_handler, K_CS, GATE_INT);

	idt_set_gate(30, exc_30_security, K_CS, GATE_INT);
	idt_set_gate(31, exc_generic_handler, K_CS, GATE_INT);



	for (n = 32; n <= 254; n++)
		idt_set_gate(n, isr_generic_handler, K_CS, GATE_INT);



	set_idt_reg(idtentry, sizeof(idtentry) - 1);

}





