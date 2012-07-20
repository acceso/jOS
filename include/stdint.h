
#ifndef STDINT_H
#define STDINT_H



typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;

typedef signed char s8;
typedef signed short int s16;
typedef signed int s32;
typedef signed long int s64;



/* Word size */
#define MWORD 8

#define MACHINEBITS 8 * MWORD


#define NULL ((void *)0)


typedef u64 size_t;
typedef u32 uid_t;
typedef u32 gid_t;
typedef u64 time_t;
typedef s64 off_t;


#define WARN_UNUSED	__attribute__((warn_unused_result))



#endif /* STDINT_H */

