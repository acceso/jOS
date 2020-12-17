
#ifndef LIB_CPU_H
#define LIB_CPU_H


#include <stdint.h>



/* This will come handy */
union _cpuregs32 {
	struct {
		u32 eax;
		u32 ebx;
		u32 edx;
		u32 ecx;
	};
	struct {
		u64 eaxebx;
		u64 edxecx;
	};
	struct {
		u32 seax;
		char s[12];
		u8 c; /* This is for the string terminator */
	};
};


/* http://www.sandpile.org/ia32/cpuid.htm */
static inline void cpuid(union _cpuregs32 *r)
{
	asm volatile("cpuid\n\t"
		:"=a" (r->eax), "=b" (r->ebx),
		 "=d" (r->edx), "=c" (r->ecx)
		:"0" (r->eax));
}




static inline u64 msr_read(u32 msr)
{
	u32 l, h;

	asm volatile(
		"rdmsr"
		: "=a" (l), "=d" (h)
		: "c" (msr));

	return ((u64)h << 32) | l;
}




static inline void msr_write(u32 msr, u64 data)
{
	asm volatile(
		"wrmsr"
		:
		: "c" (msr), "a"(data & 0xffffffff), "d" (data >> 32)
		: "memory");
}


static inline void iowait(void)
{
	asm volatile(
		"jmp 1f\n\t"
		"1:\n\t"
		"jmp 1f\n\t"
		"1:\n\t"
		);
}

static inline void yield(void)
{
	asm volatile("nop\n\t");
}


#endif /* LIB_CPU_H */


