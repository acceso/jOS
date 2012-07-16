#ifndef TRAPS_H
#define TRAPS_H


#include <stdint.h>
#include <stdio.h>



/* interrupt gates: if = 0 (no more interrupts until iret)
 * trap gates: leave if untouched */
#define GATE_INT  0x8e
#define GATE_TRAP 0x8f



#define __isr__		\
	__attribute__ ((aligned(16))) static void



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



/* The "nop"s are to skip the function prologue, see traps.c.
 * push $0 is a fake error code.
 * The sub $16, %rsp is needed for the frame parameter to match. */
#define intr_enter()			\
	asm volatile (			\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		pushaq()		\
		"sub $16, %rsp\n\t" 	\
		"cld\n\t"		\
		)

#define intr_err_enter()		\
	asm volatile (			\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		pushaq()		\
		"sub $16, %rsp\n\t" 	\
		"cld\n\t"		\
		)




#define intr_exit(_n)			\
	asm volatile (			\
		"addq $16, %rsp\n\t" 	\
		popaq()			\
		"iretq\n\t"		\
		)

#define intr_err_exit(_n)		\
	asm volatile (			\
		"addq $16, %rsp\n\t" 	\
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
} __attribute__((__packed__));



struct intr_frame_err {
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
	u64 ecode;
	u64 retrip;
	u64 cs;
	u64 rflags;
	u64 retrsp;
	u64 ss;
} __attribute__((__packed__));



static inline void
stack_frame_dump (struct intr_frame *r)
{
	kprintf ("r11    (%llp)\n", r->r11);
	kprintf ("r10    (%llp)\n", r->r10);
	kprintf ("r9     (%llp)\n", r->r9);
	kprintf ("r8     (%llp)\n", r->r8);
	kprintf ("rdi    (%llp)\n", r->rdi);
	kprintf ("rsi    (%llp)\n", r->rsi);
	kprintf ("rdx    (%llp)\n", r->rdx);
	kprintf ("rcx    (%llp)\n", r->rcx);
	kprintf ("rax    (%llp)\n", r->rax);
	kprintf ("retrip (%llp)\n", r->retrip);
	kprintf ("cs     (%llp)\n", r->cs);
	kprintf ("rflags (%llp)\n", r->rflags);
	kprintf ("retrsp (%llp)\n", r->retrsp);
	kprintf ("ss     (%llp)\n", r->ss);
	kprintf ("\n\n\n");
}


static inline void
stack_frame_err_dump (struct intr_frame_err *r)
{
	kprintf ("r11    (%llp)\n", r->r11);
	kprintf ("r10    (%llp)\n", r->r10);
	kprintf ("r9     (%llp)\n", r->r9);
	kprintf ("r8     (%llp)\n", r->r8);
	kprintf ("rdi    (%llp)\n", r->rdi);
	kprintf ("rsi    (%llp)\n", r->rsi);
	kprintf ("rdx    (%llp)\n", r->rdx);
	kprintf ("rcx    (%llp)\n", r->rcx);
	kprintf ("rax    (%llp)\n", r->rax);
	kprintf ("ecode  (%llp)\n", r->ecode);
	kprintf ("retrip (%llp)\n", r->retrip);
	kprintf ("cs     (%llp)\n", r->cs);
	kprintf ("rflags (%llp)\n", r->rflags);
	kprintf ("retrsp (%llp)\n", r->retrsp);
	kprintf ("ss     (%llp)\n", r->ss);
	kprintf ("\n\n\n");
}




void intr_install_handler (u8 num, u64 addr);
void init_exceptions (void);




#endif /* TRAPS_H */

