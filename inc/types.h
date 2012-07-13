
#ifndef TYPES_H
#define TYPES_H


#define NULL 0x0

#define MWORD 8


#ifndef _ASM /* Not for assembly */

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;

typedef signed char s8;
typedef signed short int s16;
typedef signed int s32;
typedef signed long int s64;

typedef u64 size_t;

#endif /* _ASM */

#define __pa(_x) (u64 *)((u64)(_x) - K_PAGE_OFFSET)
#define __va(_x) (u64 *)((u64)(_x) + K_PAGE_OFFSET)


#endif /* TYPES_H */

