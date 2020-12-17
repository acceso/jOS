
#ifndef VM_CPU_H
#define VM_CPU_H


#include <stdint.h>

#include <lib/cpu.h>

#include <kernel/intr.h>

#include "syscall.h"



struct cpu {
	u8 id;
	struct _lapic *lapic;
	u64 hz;
};


struct sys {
	struct cpu cpu[1];
	u64 busspeed;
	u8 has8259;
};


struct gdt {
	u16 limit;
	u16 base1;
	u8 base2;
	u8 type;
	u8 limit2;
	u8 base3;
	u32 base4;
	u32 reserved;
} __attribute__((__packed__));


#define MSR_STAR	0xc0000081
#define MSR_LSTAR	0xc0000082
#define MSR_SFMASK	0xc0000048



/* TODO: use the flag mask on MSR_SFMASK
 * and maybe some register cleaning. */
#define usermode_jump(_addr)					\
	do {							\
		msr_write(MSR_STAR, ((u64)U_CS)<<48 		\
				   | ((u64)K_CS)<<32 );		\
		msr_write(MSR_LSTAR, (u64)syscall_dispatch);	\
		asm volatile("mov %0, %%rcx\n\t"		\
			"sysretq\n\t" : : "m"(_addr));		\
	} while (0)



void init_cpu();



#endif /* VM_CPU_H */

