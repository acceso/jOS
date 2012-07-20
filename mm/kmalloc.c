
#include <stdint.h>
#include <string.h>

#include "kcache.h"
#include "kma.h"
#include "mm.h"



struct kcache *kmalloc16;
struct kcache *kmalloc32;
struct kcache *kmalloc64;
struct kcache *kmalloc128;
struct kcache *kmalloc256;
struct kcache *kmalloc512;
struct kcache *kmalloc1024;
struct kcache *kmalloc2048;
struct kcache *kmalloc4096;
struct kcache *kmalloc8192;




void *
kmalloc (size_t size)
{
	struct kcache *cache = NULL;

	if (size <= 16)
		cache = kmalloc16;
	else if (size <= 32)
		cache = kmalloc32;
	else if (size <= 64)
		cache = kmalloc64;
	else if (size <= 128)
		cache = kmalloc128;
	else if (size <= 256)
		cache = kmalloc256;
	else if (size <= 512)
		cache = kmalloc512;
	else if (size <= 1024)
		cache = kmalloc1024;
	else if (size <= 2048)
		cache = kmalloc2048;
	else if (size <= 4096)
		cache = kmalloc4096;
	else if (size <= 8192)
		cache = kmalloc8192;

	if (cache != NULL)
		return kcache_alloc (cache);

	return kma_alloc (size);
}



void *
xkmallocf (size_t size, const char *msg)
{
	void *ptr;

	ptr = kmalloc (size);
	if (ptr == NULL)
		oom (msg);

	return ptr;
}



void *
xkmallocf0 (size_t size, const char *msg)
{
	void *p;
	
	p = xkmallocf (size, msg);

	memset (p, '\0', size);

	return p;
}


void
kfree (void *ptr)
{
	/* I don't like this solution very much but this is a toy os
	 * isn't it? */
	if (kcache_free (kmalloc16, ptr) == 1)
		return;
	if (kcache_free (kmalloc32, ptr) == 1)
		return;
	if (kcache_free (kmalloc64, ptr) == 1)
		return;
	if (kcache_free (kmalloc128, ptr) == 1)
		return;
	if (kcache_free (kmalloc256, ptr) == 1)
		return;
	if (kcache_free (kmalloc512, ptr) == 1)
		return;
	if (kcache_free (kmalloc1024, ptr) == 1)
		return;
	if (kcache_free (kmalloc2048, ptr) == 1)
		return;
	if (kcache_free (kmalloc4096, ptr) == 1)
		return;
	if (kcache_free (kmalloc8192, ptr) == 1)
		return;


	kma_free (ptr);

	return;
}



void
init_kmalloc (void)
{
	return;

	kmalloc16 = kcache_create ("kmalloc-16", 16);
	kmalloc32 = kcache_create ("kmalloc-32", 32);
	kmalloc64 = kcache_create ("kmalloc-64", 64);
	kmalloc128 = kcache_create ("kmalloc-128", 128);
	kmalloc256 = kcache_create ("kmalloc-256", 256);
	kmalloc512 = kcache_create ("kmalloc-512", 512);
	kmalloc1024 = kcache_create ("kmalloc-1024", 1024);
	kmalloc2048 = kcache_create ("kmalloc-2048", 2048);
	kmalloc4096 = kcache_create ("kmalloc-4096", 4096);
	kmalloc8192 = kcache_create ("kmalloc-8192", 8192);

	return;
}



