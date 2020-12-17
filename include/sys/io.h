
#ifndef SYS_IO_H
#define SYS_IO_H


#include <stdint.h>

#include <lib/cpu.h>




static inline void outb(u16 port, u8 val)
{
	asm volatile("outb %1, %0" : : "dN"(port), "a"(val));

	iowait();

}



static inline u8 inb(u16 port)
{
	u8 ret;

	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));

	iowait();

	return ret;
}



static inline u16 inw(u16 port)
{
	u16 ret;

	asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));

	iowait();

	return ret;
}


static inline void outw(u16 port, u16 val)
{
	asm volatile("outw %1, %0" : : "dN"(port), "a"(val));

	iowait();

}





#endif /* SYS_IO_H */


