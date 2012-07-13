
#ifndef STDIO_H
#define STDIO_H


#include <inc/types.h>


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;



#define is_digit(_c) ((unsigned)(_c) - '0' <= 9)


void puts(const char *s);

void kprintf(const char *fmt, ...);

void outb(u16 port, u8 val);
u8 inb(u16 port);
u16 inw(u16 port);



#endif /* STDIO_H */

