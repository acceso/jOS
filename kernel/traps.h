#ifndef TRAPS_H
#define TRAPS_H

#include <inc/types.h>


#define GATE_INT  0x8e
#define GATE_TRAP 0x8f



#define __isr__ \
	__attribute__ ((regparm (0), aligned(8)))


#define pushaq() 		\
	"pushq %rax\n\t"	\
	"pushq %rcx\n\t"	\
	"pushq %rdx\n\t"	\
	"pushq %rbx\n\t"	\
	"pushq %rbp\n\t"	\
	"pushq %rsi\n\t"	\
	"pushq %rdi\n\t"	\
	"pushq %r8\n\t"		\
	"pushq %r9\n\t"		\
	"pushq %r10\n\t"	\
	"pushq %r11\n\t"	\
	"pushq %r12\n\t"	\
	"pushq %r13\n\t"	\
	"pushq %r14\n\t"	\
	"pushq %r15\n\t"


#define popaq() 		\
	"popq %r15\n\t"		\
	"popq %r14\n\t"		\
	"popq %r13\n\t"		\
	"popq %r12\n\t"		\
	"popq %r11\n\t"		\
	"popq %r10\n\t"		\
	"popq %r9\n\t"		\
	"popq %r8\n\t"		\
	"popq %rdi\n\t"		\
	"popq %rsi\n\t"		\
	"popq %rbp\n\t"		\
	"popq %rbx\n\t"		\
	"popq %rdx\n\t"		\
	"popq %rcx\n\t"		\
	"popq %rax\n\t"


/* Note: the addq $8 compensates the parameter. */
#define intr_enter(_n)			\
	asm volatile (			\
		"cli\n\t"		\
		"pushq $" #_n "\n\t"	\
		pushaq()		\
		"addq $8, %rsp\n\t"	\
		)


#define intr_exit(_n)			\
	asm volatile (			\
		popaq()			\
		"addq $8, %rsp\n\t"	\
		"sti\n\t"		\
		"iretq\n"		\
		)


struct intr_frame {
	u64 r15;
	u64 r14;
	u64 r13;
	u64 r12;
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
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



void init_exceptions (void);

void idt_set_gate (u8 num, u64 addr, u16 selector, u16 flags);




#endif /* TRAPS_H */

