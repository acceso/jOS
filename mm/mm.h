#ifndef MM_H
#define MM_H



#include <stdint.h>


#define PAGE_ALIGN      ~(0x200000 - 1)
#define PAGE_SIZE       0x200000
#define PAGE_OFFSET     21


#define PAGE_ALIGNED(_a) (((u64)(_a) & PAGE_ALIGN) == (u64)(_a))




/* I get 2 ranges on my bochs emulator, as I can't ask for memory yet, 
 * I reserv more elements. */
#define MMAP_ARRAY_MAX 10
struct _usablemem {
	u64 *addr;
	u64 len;
};



void init_memory (void);



#endif /* MM_H */

