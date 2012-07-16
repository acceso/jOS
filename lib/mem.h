#ifndef MEM_H
#define MEM_H


#include <stdint.h>



/* Memory barriers: */
#define serialize() asm volatile("lock; subq $0,(%rsp)")



#define __pa(_x) ((_x) - K_PAGE_OFFSET)
#define __va(_x) ((_x) + K_PAGE_OFFSET)



void *align_to (void *a, u32 p);



#endif /* MEM_H */


