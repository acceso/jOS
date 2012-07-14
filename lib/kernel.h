
#ifndef KERNEL_H
#define KERNEL_H



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


void kpanic (char *str);


#endif /* KERNEL_H */


