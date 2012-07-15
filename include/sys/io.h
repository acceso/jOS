
#ifndef SYS_IO_H
#define SYS_IO_H


#include <stdint.h>




static inline void
outb (u16 port, u8 val)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}



static inline u8
inb (u16 port)
{
	u8 ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}



static inline u16
inw (u16 port)
{
	u16 ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
} 





#endif /* SYS_IO_H */


