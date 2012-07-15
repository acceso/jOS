
#ifndef MM_KMALLOC
#define MM_KMALLOC


#include <stdint.h>



#define MEM_UNCACHED	0x1




void *kmalloc (u64 size, u16 flags);
void kfree (void *addr);

void heap_init (void);



#endif /* MM_KMALLOC */


