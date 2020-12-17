#ifndef KERNEL_TRAPS_H
#define KERNEL_TRAPS_H


#include <stdint.h>
#include <stdio.h>



/* interrupt gates: "if" = 0 (no more interrupts until iret)
 * trap gates: leave "if" untouched */
#define GATE_INT  0x8e
#define GATE_TRAP 0x8f



#define __isr__		\
	__attribute__((aligned(16))) static void



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
	"pushq %r11\n\t"



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
 * pushq $0 is a fake error code.
 * The last "sub" is needed to make the function parameter
 * (intr_frame) work. */
#define intr_enter()			\
	asm volatile(			\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"pushq $0\n\t"		\
		pushaq()		\
		"subq $16, %rsp\n\t"	\
		"cld\n\t"		\
		)

#define intr_err_enter()		\
	asm volatile(			\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		"nop; nop; nop; nop;"	\
		pushaq()		\
		"subq $16, %rsp\n\t"	\
		"cld\n\t"		\
		)



/* The addqs undo the previous subqs */
#define intr_exit(_n)			\
	asm volatile(			\
		"addq $16, %rsp\n\t"	\
		popaq()			\
		"addq $8, %rsp\n\t" 	\
		"iretq\n\t"		\
		)


#define intr_err_exit(_n)		\
	asm volatile(			\
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
	u32 ecode;
	u32 pad4;
	u64 rip;
	u8 cs;
	u8 pad0; u32 pad1;
	u64 rflags;
	u64 rsp;
	u8 ss;
	u8 pad2; u32 pad3;
} __attribute__((__packed__));



static inline void stack_frame_dump(struct intr_frame *r)
{
	kprintf("r11    (%llp)\n", r->r11);
	kprintf("r10    (%llp)\n", r->r10);
	kprintf("r9     (%llp)\n", r->r9);
	kprintf("r8     (%llp)\n", r->r8);
	kprintf("rdi    (%llp)\n", r->rdi);
	kprintf("rsi    (%llp)\n", r->rsi);
	kprintf("rdx    (%llp)\n", r->rdx);
	kprintf("rcx    (%llp)\n", r->rcx);
	kprintf("rax    (%llp)\n", r->rax);
	kprintf("rip    (%llp)\n", r->rip);
	kprintf("cs     (0x%x)\n", r->cs);
	kprintf("rflags (%llp)\n", r->rflags);
	kprintf("rsp    (%llp)\n", r->rsp);
	kprintf("ss     (0x%x)\n", r->ss);
	kprintf("\n\n");
}




void intr_install_handler(u8 num, void *addr);
void init_exceptions(void);




#endif /* KERNEL_TRAPS_H */

