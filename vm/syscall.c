

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
		pushaq()
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

	kpanic ("Shouldn't be reached as we never return normally from syscall.");
}



void
init_syscall (void)
{
	/* I want to skip the function prologue: */
	syscall_dispatch = syscall_dispatch_real + 16;
}




