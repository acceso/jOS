#ifndef TRAPS_H
#define TRAPS_H


#include <stdint.h>


/* interrupt gates: if = 0 (no more interrupts until iret)
 * trap gates: leave if untouched */
#define GATE_INT  0x8e
#define GATE_TRAP 0x8f



#define __isr__		\
	__attribute__ ((aligned(8))) static void



/* Note: gcc pushes %rbx automátically! */
#define pushaq() 		\
	"pushq %rax\n\t"	\
	"pushq %rcx\n\t"	\
	"pushq %rdx\n\t"	\
	"pushq %rbx\n\t"	\
	"pushq %rsi\n\t"	\
	"pushq %rdi\n\t"	\
	"pushq %r8\n\t"		\
	"pushq %r9\n\t"		\
	"pushq %r10\n\t"	\
	"pushq %r11\n\t"	\


/* Note: gcc can't pop rbx because the iret comes before! */
#define popaq() 		\
	"popq %r11\n\t"		\
	"popq %r10\n\t"		\
	"popq %r9\n\t"		\
	"popq %r8\n\t"		\
	"popq %rdi\n\t"		\
	"popq %rsi\n\t"		\
	"popq %rbx\n\t"		\
	"popq %rdx\n\t"		\
	"popq %rcx\n\t"		\
	"popq %rax\n\t"



/* This deserves an explanation, see traps.c, idt_set_gate definition */
/* There are two rbp pushes because gcc needs rip + rbp */
#define intr_enter()			\
	asm volatile (			\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		pushaq()		\
		"pushq %rbp\n\t"	\
		"pushq %rbp\n\t"	\
		"movq %rsp, %rbp\n\t"	\
		"cld\n\t"		\
		)



#define intr_exit(_n)			\
	asm volatile (			\
		"addq $16, %rsp\n\t"	\
		popaq()			\
		"iretq\n\t"		\
		)



struct intr_frame {
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rdi;
	u64 rsi;
	u64 rbx;
	u64 rdx;
	u64 rcx;
	u64 rax;
	u64 retrip;
	u64 cs;
	u64 rflags;
	u64 retrsp;
	u64 ss;
};



struct exceptp_frame {
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rdi;
	u64 rsi;
	u64 rdx;
	u64 rcx;
	u64 rax;
	u64 ecode;
	u64 retrip;
	u64 cs;
	u64 rflags;
	u64 retrsp;
	u64 ss;
};



void init_exceptions (void);

void idt_set_gate (u8 num, u64 addr, u16 selector, u16 flags);



#endif /* TRAPS_H */

