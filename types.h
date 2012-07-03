
#ifndef TYPES_H
#define TYPES_H


#if ARCH == X86

#define ARCHWIDTH 32

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;
typedef unsigned long long int u64;



#elif ARCH == X86-64

#define ARCHWIDTH 64

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;



#endif /* ARCH */



typedef u64 size_t;


#endif /* TYPES_H */

