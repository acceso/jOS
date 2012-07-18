
#ifndef MM_KCACHE_H
#define MM_KCACHE_H


#include <stdint.h>

#include <lib/list.h>



struct kcache;


struct kcache *kcache_create (const char *name, size_t size);
void kcache_destroy (struct kcache *cache);

void *kcache_alloc (struct kcache *cache);
u8 kcache_free (struct kcache *cache, void *buf);



#endif /* MM_KCACHE_H */


