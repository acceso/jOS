#ifndef INTERRUPT_H
#define INTERRUPT_H


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




void init_exceptions (void);




#endif /* INTERRUPT_H */

