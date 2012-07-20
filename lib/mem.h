#ifndef LIB_MEM_H
#define LIB_MEM_H


#include <stdint.h>



/* Memory barriers: */
#define serialize() asm volatile("lock; subq $0,(%rsp)")



#define __pa(_x) ((void *)(((u64)_x) - K_PAGE_OFFSET))
#define __va(_x) ((void *)(((u64)_x) + K_PAGE_OFFSET))




#define containerof(_ptr, _type, _member) \
	(_type *)((char *)(_ptr) - __builtin_offsetof (_type, _member))


void *align_to (void *a, u32 p);



#endif /* LIB_MEM_H */


