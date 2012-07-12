
#ifndef TYPES_H
#define TYPES_H


#define NULL 0x0

#define MWORD 8

#define __pa(_x) ((_x) - K_PAGE_OFFSET)
#define __va(_x) ((_x) + K_PAGE_OFFSET)

#ifndef _ASM /* Not for assembly */

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;

typedef u64 size_t;

#endif /* _ASM */


#endif /* TYPES_H */

