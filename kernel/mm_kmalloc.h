
#ifndef MM_KMALLOC
#define MM_KMALLOC

#include <inc/types.h>


void *kmalloc (u64 size);
void kfree (void *addr);


#endif /* MM_KMALLOC */


