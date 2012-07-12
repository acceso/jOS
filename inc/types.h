
#ifndef TYPES_H
#define TYPES_H


#if ARCH == X86

#define MWORD 4

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;
typedef unsigned long long int u64;



#elif ARCH == X86-64

#define MWORD 8

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;



#endif /* ARCH */



typedef u64 size_t;

#define NULL 0x0

#endif /* TYPES_H */

