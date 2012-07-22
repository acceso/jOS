

#include <stdint.h>

#include <lib/kernel.h>

#include <kernel/task.h>
#include <kernel/traps.h>

#include "syscall.h"



static void *syscall_table[__NR_syscall_num];


s8
syscall_register (u16 num, void *handler)
{
	if (num >= __NR_syscall_num)
		return -1;

	syscall_table[num] = handler;

	return 0;
}




void (*syscall_dispatch) (void);


void
syscall_dispatch_real (void)
{
	asm volatile (
		"nop; nop; nop; nop;"
		"nop; nop; nop; nop;"
		"nop; nop; nop; nop;"
		"nop; nop; nop; nop;\n\t"
		/* If syscall is out of bounds: */
		"cmpq $" stringify (__NR_syscall_num) ", %rax\n\t"
		"jae 1f\n\t"
		/* TODO: This is a hack because it's easier to do it here. 
		 * I'm not sure of the right solution as probably interrupts
		 * should not be allowed until a safer place... */
		"cmpq $" stringify (__NR_exit) ", %rax\n\t"
		"je 2f\n\t"
		pushaq()
		"2:\n\t"
	);

	/* This code can't be merged with the previous one,
	 * because gcc fills the registers before any asm code. */
	asm volatile (
		/* syscall number << 3 because everyone is 8 bytes wide: */
		"shl $3, %%rax\n\t"
		"addq %0, %%rax\n\t"
		"call *(%%rax)\n\t"
		: : "c"(syscall_table)
	);

	/* This can't be merged because popaq() uses %rax instead of %%rax: */
	asm volatile (
		popaq()
		"1:\n\t" 
		"sysretq\n\t"
	);

	kernel_idle();
}



void
init_syscall (void)
{
	/* I want to skip the function prologue: */
	syscall_dispatch = syscall_dispatch_real + 16;
}




