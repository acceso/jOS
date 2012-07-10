

#include <inc/types.h>
#include <drivers/vga.h>
#include <lib/io.h>

#include "boot.h"
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


struct intr_frame {
	u64 rdi;
	u64 rsi;
	u64 rbp;
	u64 rbx;
	u64 rdx;
	u64 rcx;
	u64 rax;
	u64 intnum;
	u64 errcode;
	u64 retrip;
	u64 cs;
	u64 rflags;
	u64 retrsp;
	u64 ss;
};


void
isr_handler (struct intr_frame r)
{
	kprintf ("Excepcion %d!\n", r.intnum);
}


#define ISR(_n)						\
__attribute__ ((regparm (0), aligned(8))) void do_isr ## _n (void) 	\
{							\
asm volatile (						\
	"cli\n\t"					\
	"pushq $0\n\t"					\
	"pushq $" #_n "\n\t"				\
	pushaq()					\
	"call isr_handler\n\t"				\
	popaq()						\
	"addq $16, %rsp\n\t"				\
	"sti\n\t"					\
	"iretq\n");					\
}

#define ISR_errcode(_n)					\
__attribute__ ((regparm (0), aligned(8))) void do_isr ## _n (void) 	\
{							\
asm volatile(						\
	"cli\n\t"					\
	"pushq $" #_n "\n\t"				\
	pushaq()					\
	"call isr_handler\n\t"				\
	popaq()						\
	"addq $16, %rsp\n\t"				\
	"sti\n\t"					\
	"iretq\n");					\
}




ISR(0)
ISR(1)
ISR(2)
ISR(3)
ISR(4)
ISR(5)
ISR(6)
ISR(7)
ISR_errcode(8)
ISR(9)
ISR_errcode(10)
ISR_errcode(11)
ISR_errcode(12)
ISR_errcode(13)
ISR_errcode(14)
ISR(15)
ISR(16)
ISR(17)
ISR(18)
ISR(19)
ISR(20)
ISR(21)
ISR(22)
ISR(23)
ISR(24)
ISR(25)
ISR(26)
ISR(27)
ISR(28)
ISR(29)
ISR(30)
ISR(31)



static void
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

	idt_set_gate (0, (u64)&do_isr0, K_CS, GATE_INT);
	idt_set_gate (1, (u64)&do_isr1, K_CS, GATE_INT);
	idt_set_gate (2, (u64)&do_isr2, K_CS, GATE_INT);
	idt_set_gate (3, (u64)&do_isr3, K_CS, GATE_INT);
	idt_set_gate (4, (u64)&do_isr4, K_CS, GATE_INT);
	idt_set_gate (5, (u64)&do_isr5, K_CS, GATE_INT);
	idt_set_gate (6, (u64)&do_isr6, K_CS, GATE_INT);
	idt_set_gate (7, (u64)&do_isr7, K_CS, GATE_INT);
	idt_set_gate (8, (u64)&do_isr8, K_CS, GATE_INT);
	idt_set_gate (9, (u64)&do_isr9, K_CS, GATE_INT);
	idt_set_gate (10, (u64)&do_isr10, K_CS, GATE_INT);
	idt_set_gate (11, (u64)&do_isr11, K_CS, GATE_INT);
	idt_set_gate (12, (u64)&do_isr12, K_CS, GATE_INT);
	idt_set_gate (13, (u64)&do_isr13, K_CS, GATE_INT);
	idt_set_gate (14, (u64)&do_isr14, K_CS, GATE_INT);
	idt_set_gate (15, (u64)&do_isr15, K_CS, GATE_INT);
	idt_set_gate (16, (u64)&do_isr16, K_CS, GATE_INT);
	idt_set_gate (17, (u64)&do_isr17, K_CS, GATE_INT);
	idt_set_gate (18, (u64)&do_isr18, K_CS, GATE_INT);
	idt_set_gate (19, (u64)&do_isr19, K_CS, GATE_INT);
	idt_set_gate (20, (u64)&do_isr20, K_CS, GATE_INT);
	idt_set_gate (21, (u64)&do_isr21, K_CS, GATE_INT);
	idt_set_gate (22, (u64)&do_isr22, K_CS, GATE_INT);
	idt_set_gate (23, (u64)&do_isr23, K_CS, GATE_INT);
	idt_set_gate (24, (u64)&do_isr24, K_CS, GATE_INT);
	idt_set_gate (25, (u64)&do_isr25, K_CS, GATE_INT);
	idt_set_gate (26, (u64)&do_isr26, K_CS, GATE_INT);
	idt_set_gate (27, (u64)&do_isr27, K_CS, GATE_INT);
	idt_set_gate (28, (u64)&do_isr28, K_CS, GATE_INT);
	idt_set_gate (29, (u64)&do_isr29, K_CS, GATE_INT);
	idt_set_gate (30, (u64)&do_isr30, K_CS, GATE_INT);
	idt_set_gate (31, (u64)&do_isr31, K_CS, GATE_INT);

	/* TODO: en lugar de reusar, poner uno generico para estas,
	   aunque bueno, la solución es tener uno genérico para todas...
	   Otra: xq solo me deja hasta 254?? */
	for (n = 32; n <= 254; n++)
		idt_set_gate (n, (u64)&do_isr20, K_CS, GATE_INT);

	set_idt_reg((u64) idtentry, sizeof(idtentry) - 1);

}





