

/*
 * Design:
 * - Ask for memory to the kma, see cache_get_size() to see how much.
 * - Place a kcache struct and one slab struct just afterwards.
 * - The slab struct has a bitmap at the end.
 * - When giving memory, locate the first not set bit, set it and return
 *   a pointer to the object it represents.
 * - When freeing memory, locate the bit for a memory address and unset it.
 * - If the slab runs out of memory, ask for a new slab twice as big.
 * 
 * 
 * Improvements:
 * - (FIXME) Important: if the kma ask for a block because it needs it for a
 *   busy_block, it'll result in a recursive call. This needs to be fixed:
 *   add a new free_block when we only have 5% of space left and free it when
 *   we have 10%. Use slabs_empty which it doesn't need a list. Remember not
 *   to ask for a new one if slabs_empty is not NULL.
 * - Add ctor y dtor for constructors and destructors. I don't need them now.
 * - Add a pointer to the last reserved object. It'll speed the search of a
 *   free object.
 * - Make sure we're always picking the oldest cache. It should be just adding
 *   the slabs at the end of the list, and giving elements from the start.
 *
 */


#include <stdint.h>
#include <stdio.h>

#include <lib/kernel.h>
#include <lib/bitset.h>

#include "mm.h"
#include "kma.h"
#include "kcache.h"



struct slab {
	struct list_head l;
	/* This points to the first free piece of memory after the bitmap */
	void *first_element;
	u32 objcount;
	u32 objused;
	/* See http://gcc.gnu.org/onlinedocs/gcc/Zero-Length.html */
	union {
		u8 usedb[0];
		u64 used[0];
	};
};


struct kcache {
	struct list_head slabs_full;
	struct list_head slabs_notfull;

	struct slab *first_slab;

	/* number of objects allocated and freed */
	size_t total_alloc;
	size_t total_freed;

	/* Times we've called kma_alloc / kma_free
	 * not counting the first one. */
	size_t kma_alloc;
	size_t kma_free;

	/* Size of each object for this cache. */
	u32 objsz;

	/* Size of the last slab added */
	u32 last_slab;
};




static void
slab_init (void *addr, size_t sz, size_t osz)
{
	struct slab *slab;
	u32 bmasksize;
	u16 i = 0;

	slab = addr;

	/* I hate this recursive problem... */
	bmasksize = ((sz - sizeof (struct slab) - (((sz - sizeof (struct slab)) / osz) >> 3)) / osz) >> 3;
	slab->objcount = (sz - sizeof (struct slab) - bmasksize) / osz;

	while (i < (slab->objcount >> 6))
		slab->used[i++] = 0;

	slab->first_element = slab->used + i + 1;

	slab->objused = 0;

	return;
}



/* kma_alloc works in powers of two; a smaller allocation would be wasteful. 
 * After looking at:
 * perl -lane '$a+=$F[1]*$F[3];END{ print +($a-2)/$. }' /proc/slabinfo
 * I believe 64 KB is a sane default for the first allocation. 
 * It could be raised to 128KB if there is a lot of ram...
 * In practice I'll use: */
static u32
cache_get_size (size_t objsz)
{
	u16 sz;

	objsz >>= 10;

	if (objsz <= 8)
		sz = 64;
	else if (objsz <= 16)
		sz = 128;
	else if (objsz <= 32)
		sz = 256;
	else
		sz = 512;

	return sz << 10;
}



/* Creates a new cache for elements sized "size". */
struct kcache *
kcache_create (const char *name, size_t objsz)
{
	struct kcache *cache;
	struct slab *slab;
	u32 sz;

	/* I don't expect objects bigger than 8-16 KB... so for now, 
	 * the object size is limited.
	 * size << 5 limits the object size to 64 KB (if PAGE_SIZE == 2 MB) */
	if ((objsz << 5) > PAGE_SIZE)
		return NULL;


	sz = cache_get_size (objsz);

	cache = (struct kcache *)kma_alloc (sz);
	if (cache == NULL)
		oom (__func__);

	list_init (&cache->slabs_full);
	list_init (&cache->slabs_notfull);
	cache->total_alloc = 0;
	cache->total_freed = 0;
	cache->kma_alloc = 0;
	cache->kma_free = 0;
	cache->objsz = objsz;
	cache->last_slab = sz;

	slab = align_to ((void *)cache + sizeof (struct kcache), MWORD);

	cache->first_slab = slab;

	slab_init (slab, sz - sizeof (struct kcache), objsz);

	list_add (&cache->slabs_notfull, &slab->l);

	return cache;
}



static void
kcache_free_slab (struct kcache *cache, struct slab *slab)
{
	/* The first slab can't be freed */
	if (slab == cache->first_slab)
		return;

	list_del (&slab->l);

	cache->kma_free++;

	kma_free (slab);
}



void
kcache_destroy (struct kcache *cache)
{
	struct slab *slab;

	list_foreach_entry (slab, &cache->slabs_full, l)
		kcache_free_slab (cache, slab);

	list_foreach_entry (slab, &cache->slabs_notfull, l)
		kcache_free_slab (cache, slab);

}



static void
kcache_add_slab (struct kcache *cache)
{
	struct slab *slab;

	if (cache->last_slab > PAGE_SIZE)
		cache->last_slab <<= 1;

	slab = (struct slab *)kma_alloc (cache->last_slab);
	if (slab == NULL)
		oom (__func__);

	cache->kma_alloc++;

	slab_init (slab, cache->last_slab, cache->objsz);

	list_add (&cache->slabs_notfull, &slab->l);
}



void *
kcache_alloc (struct kcache *cache)
{
	struct slab *slab;
	u16 i = 0;
	s8 bit;

restart:
	if (list_empty (&cache->slabs_notfull))
		kcache_add_slab (cache);

	slab = containerof (list_next (&cache->slabs_notfull),
		struct slab, l);

	/* Each increment of i advances 64 bits or 2^6 */
	while (i < (slab->objcount >> 6)) {
		if (slab->used[i] != ~0L)
			break;

		i++;
	}

	if (i > slab->objcount >> 6)
		/* Shouldn't reach here because we've checked before
		 * there are free objects... anyway: */
		goto restart;

	bit = bitscan_right (~slab->used[i]);
	bitset (slab->used + i, bit);

	slab->objused++;

	cache->total_alloc++;
	
	if (slab->objused == slab->objcount) {
		list_del (&slab->l);
		list_add (&cache->slabs_full, &slab->l);
	}
		

	return slab->first_element + i * (cache->objsz << 6) + bit * cache->objsz;
}



static inline struct slab *
kcache_get_slab (struct kcache *cache, struct list_head *l, void *buf)
{
	struct slab *s;

	list_foreach_entry (s, l, l) {
		if (buf >= (void *)s->first_element &&
		    buf <= (void *)s->first_element + s->objcount * cache->objsz)
			return s;
	}

	return NULL;
}



u8
kcache_free (struct kcache *cache, void *buf)
{
	struct slab *slab;
	

	slab = kcache_get_slab (cache, &cache->slabs_notfull, buf);
	if (slab == NULL) {
		slab = kcache_get_slab (cache, &cache->slabs_full, buf);
		if (slab == NULL)
			return 0;
	}


	bitclearptr (slab->usedb, (buf - slab->first_element) / cache->objsz);

	slab->objused--;
	cache->total_freed++;

	/* Don't change the order,
	 * there can theoretically exist a full slab holding 1 element. */
	if (slab->objused == 0)
		kcache_free_slab (cache, slab);
	else if (slab->objused == slab->objcount - 1) {
		list_del (&slab->l);
		list_add (&cache->slabs_notfull, &slab->l);
	}

	return 1;
}



