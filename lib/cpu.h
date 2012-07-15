
#ifndef LCPU_H
#define LCPU_H


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
		u32 eax;
		char s[12];
		u8 c; /* This is for the string termiator */
	};
};


static inline void
cpuid (union _cpuregs32 *r)
{
	asm volatile ("cpuid\n\t"
		:"=a" (r->eax), "=b" (r->ebx),
		 "=d" (r->edx), "=c" (r->ecx)
		:"0" (r->eax));
}




static inline u64
msr_read (u32 msr)
{
	u32 l, h;

	asm volatile (
		"rdmsr"
		: "=a" (l), "=d" (h)
		: "c" (msr));

	return ((u64)h << 32) | l;
}




static inline void
msr_write (u32 msr, u64 data)
{
	asm volatile (
		"wrmsr"
		:
		: "c" (msr), "a"(data & 0xffffffff), "d" (data >> 32)
		: "memory");
}



static inline void
yield (void)
{
	asm volatile ("nop\n\t");
}


#endif /* LCPU_H */


