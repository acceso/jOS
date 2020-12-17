#ifndef MM_KMA_H
#define MM_KMA_H


#include <stdint.h>



void *kma_alloc(size_t size);
void kma_free(void *addr);

void kma_init(void);



#endif /* MM_KMA_H */


