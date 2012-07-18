
#ifndef MM_KMALLOC_H
#define MM_KMALLOC_H




void *kmalloc (size_t size);
void kfree (void *ptr);

void init_kmalloc (void);


#endif /* MM_KMALLOC_H */


