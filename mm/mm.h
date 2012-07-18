#ifndef MM_MM_H
#define MM_MM_H



#include <stdint.h>


#define PAGE_ALIGN	~(0x200000 - 1)
#define PAGE_SIZE	0x200000
#define PAGE_OFFSET	21


#define PAGE_ALIGNED(_a) (((u64)(_a) & PAGE_ALIGN) == (u64)(_a))



u64 get_mm_range (void **addr, u16 n);

void oom (const char *str);
void init_memory (void);



#endif /* MM_MM_H */

