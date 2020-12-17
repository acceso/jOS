
#ifndef MM_KMALLOC_H
#define MM_KMALLOC_H


#define xkmalloc(sz) xkmallocf(sz, __func__)
#define xkmalloc0(sz) xkmallocf0(sz, __func__)

void *kmalloc(size_t size);
void *xkmallocf(size_t size, const char *msg);
void *xkmallocf0(size_t size, const char *msg);
void kfree(void *ptr);

void init_kmalloc(void);


#endif /* MM_KMALLOC_H */


