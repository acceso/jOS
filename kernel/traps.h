#ifndef TRAPS_H
#define TRAPS_H

#include <inc/types.h>


#define GATE_INT  0x8e
#define GATE_TRAP 0x8f



#define pushaq() 		\
	"pushq %rax\n\t"	\
	"pushq %rcx\n\t"	\
	"pushq %rdx\n\t"	\
	"pushq %rbx\n\t"	\
	"pushq %rbp\n\t"	\
	"pushq %rsi\n\t"	\
	"pushq %rdi\n\t"

#define popaq() 		\
	"popq %rdi\n\t"		\
	"popq %rsi\n\t"		\
	"popq %rbp\n\t"		\
	"popq %rbx\n\t"		\
	"popq %rdx\n\t"		\
	"popq %rcx\n\t"		\
	"popq %rax\n\t"



#define ISR(_n)						\
__attribute__ ((regparm (0), aligned(8))) void do_isr ## _n (void) 	\
{							\
asm volatile (						\
	"cli\n\t"					\
	"pushq $" #_n "\n\t"				\
	pushaq()					\
	"call isr_handler\n\t"				\
	popaq()						\
	"addq $8, %rsp\n\t"				\
	"sti\n\t"					\
	"iretq\n");					\
}


void init_exceptions (void);

void idt_set_gate (u8 num, u64 addr, u16 selector, u16 flags);




#endif /* TRAPS_H */

