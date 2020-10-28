
/* 
 * Although not hard, this code it's not trivial. I document it here:
 * 
 * When asked for memory:
 * - Look in kma.free[] for the smallest "big enough" block.
 * - If it doesn't exist, call get_one_page () to get more memory.
 * - Save the size of the used space into a tree: kma.used
 * - If the memory we've asked for, it smaller than half a page, 
 *   the free space it's given back to the free list.
 *   See give_first_and_split_buddies(), which uses blocksizes[].
 * This step can be improved because if we're asked 1MB + 1byte and
 * the page size is 2MB, 0.99MB are wasted. I believe it's not worth it
 * because this layer will be used below the slabs.
 * 
 * When freeing memory:
 * - Look for the size into the tree kma.used.
 * - Remove the node from the tree.
 * - Link the node into the free list.
 * - Coalescence the free list and if an element is free in the biggest
 *   chunk (which is page sized), free the page.
 * This step can be improved because if the client repeatedly ask and frees a 
 * block which requires a new page allocation, a lot of work will be done
 * dividing and coalescencing the block and the page. Anyway, it shouldn't
 * happen a lot and it's just an internal speed improvement.
 *
 * More improvements:
 * - The most importan failure is we need memory for the nodes of the tree,
 *   but we don't have an allocator as we are the allocator... I will work
 *   the slabs layer before giving a solution.
 * - Use a linked list of pages. Se the previous point.. I'll add it later.
 * - Add stats: nº of nodes linked into the free lists (I can get free and
 *   used memory with this information).
 * 
 */


#include <stdint.h>
#include <stdio.h>

#include <lib/bitset.h>
#include <lib/kernel.h>
#include <lib/list.h>
#include <lib/mem.h>
#include <lib/math.h>
#include <lib/tree.h>

#include "mm.h"
#include "phys.h"
#include "kma.h"
#include "kcache.h"
#include "kmalloc.h"


static void kma_mark_free (void *addr, u8 idx);




#define BLOCKSIZES_SZ	(sizeof (blocksizes) / sizeof (typeof (blocksizes[0])))

/* Note the last one has to be PAGE_SIZE. */
static const u32 blocksizes[] = {
	1<<14, 1<<15, 1<<16, 1<<17, 1<<18, 1<<19, 1<<20, 1<<21
};



struct free_block {
	struct list_head l;
};

struct busy_block {
	struct tnode t;
	void *addr;
	u8 size_idx;
};

struct page_block {
	struct list_head l;
	void *addr;
};

static struct kma {
	/* This list "pages" won't be used yet,
	 * it complicates things and it's dispensable. */
	struct list_head pages;
	u16 npages;
	struct free_block free[BLOCKSIZES_SZ];
	struct tree used;
} kma;




/* Gives the index needed into kma.free[] to get at least 2^power bytes */
static u8
kma_blocksizes_idx (u8 power)
{
	s8 i = 0;

	while (blocksizes[i] < (1 << power))
		i++;

	return i;
}



static struct kcache *busy_block_cache;
/* This breaks the chicken-egg problem. */
static u8 kma_early_init_done;
static struct busy_block bblock[64];
static u64 bblock_bmask;

static void *
kma_early_alloc_next_busy (void)
{
	s8 bit;

	if (kma_early_init_done)
		return kcache_alloc (busy_block_cache);

	bit = bitscan_right (~bblock_bmask);
	if (bit == -1)
		kpanic ("Oops! kma memory exhausted :(\n");

	bitset (&bblock_bmask, bit);

	return &bblock[bit];
}


static void
kma_early_free_busy (void *addr)
{
	/* The most likely situation is addr > bblock[last] */
	if (addr > (void *)&bblock[63] || addr < (void *)bblock)
		kcache_free (busy_block_cache, addr);
	else
		bitclear (&bblock_bmask, 
			(addr - (void *)bblock) / sizeof (struct busy_block));
}



static inline void
kma_free_page (void *addr)
{
	kma.npages--;

	free_page (addr);
}



static u8
kma_pool_contraction (void)
{
	u8 i = 0;

	/* Can't use list_foreach because the list can be modified
	 * and I don't have something like list_foreach_safe */
	while (!list_empty (&kma.free[BLOCKSIZES_SZ - 1].l)) {
		kma_free_page ((void *)containerof (
			list_pop (&kma.free[BLOCKSIZES_SZ - 1].l), struct free_block, l));
		i++;
	}

	return i;
}



/* Expands the pool of memory with a new page. */
static void *
kma_pool_expand_one (void)
{
	void *p;
	struct free_block *nb;

	p = get_one_page ();
	if (p == NULL)
		oom (__func__);


	nb = (struct free_block *)p;

	kma.npages++;

	list_add (&kma.free[kma_blocksizes_idx (PAGE_OFFSET)].l, &nb->l);

	return p;
}



static inline void
kma_merge_next (struct list_head *which, u8 idx)
{
	struct list_head *n;

	n = list_next (which);

	list_del (n);
	list_del (which);

	/* Note list_del just unlinks the node. */
	kma_mark_free (containerof (which, struct free_block, l), idx + 1);
}



static inline u8
kma_try_merge_next (struct list_head *which, u8 idx)
{
	struct free_block *b = containerof (which, struct free_block, l);

	/* Next of last is head. Not mergeable. */
	if (containerof(b->l.next, struct free_block, l) == &kma.free[idx])
		return 0;

	/* See if the blocks are "mergeable".
	 * The first one needs to be a multiple of the next block. */
	if ((u64)b % blocksizes[idx + 1] != 0)
		return 0;

	/* See if the next one on the list is adjacent. */
	if ((u64)b + blocksizes[idx] != 
		(u64)containerof (list_next (which), struct free_block, l))
		return 0;


	kma_merge_next (which, idx);


	return 1;
}



/* If we have two consecutive free pieces of memory of the same size,
 * this function coalescences them into one chunk twice as big. 
 * If it is a block big enough, it frees the page.
 */
static inline void
kma_try_merge (u8 idx)
{
	struct list_head *c, *tmp;

	/* The biggest block is not mergeable, but it can be freed */
	if (idx == BLOCKSIZES_SZ - 1)
		kma_pool_contraction ();
	else
		/* I can't traverse safely because elements can be removed.
		 * list_foreach_safe wouldn't work because two elements are
		 * removed. This should work without messing with the lists */
		list_foreach (c, &kma.free[idx].l) {
			/* If kma_try_merge_next doesn't remove elements, tmp
			 * is not used. Otherwise, list has 3 or more elements
			 * (the head, the two being removed and maybe more).
			 * IOW: tmp will point to the head or to the next
			 * mergeable element, and this is what we want */
			tmp = list_next (list_next (c));

			if (kma_try_merge_next (c, idx) == 0)
				continue;

			c = tmp;
		}

	return;
}



/* Frees memory at address "addr" which is 2^power big. */
static void
kma_mark_free (void *addr, u8 idx)
{
	struct list_head *new = (struct list_head *)addr;
	struct list_head *c;

	list_foreach (c, &kma.free[idx].l)
		if (c < new)
			break;

	list_add (c, new);


	/* Looking at gdb stack frames, I'm using 2940 bytes, looks too much.
	 * Let's try it and see what happens. Should be less with inlining. */
	kma_try_merge (idx);

	return;
}



/* Used to give memory back to the pool. */
void
kma_free (void *addr)
{
	struct busy_block key;
	struct busy_block *busy;
	struct tnode *t;

	key.addr = addr;

	t = tree_lookup (&kma.used, &key.t);
	if (t == NULL) {
		kputs ("Tried to free an non existent block.\n");
		return;
	}

	busy = containerof (t, struct busy_block, t);

	tree_detach (&kma.used, &busy->t);

	kma_mark_free (addr, busy->size_idx);

	kma_early_free_busy (busy);
}



/* Retrieves the first element from the linked list of free blocks
 * of a size at least as big as we've been asked.
 * If the block is twice as big as needed, the second half
 * is added back to the pool. */
static inline void *
give_first_and_split_buddies (u8 idx, u8 power)
{
	struct free_block *freeb;
	struct busy_block *busy;

	freeb = containerof (list_pop (&kma.free[idx].l),
		struct free_block, l);

	busy = kma_early_alloc_next_busy ();
	if (busy == NULL)
		kpanic ("Not enough early memory.\n");

	busy->addr = freeb;
	busy->size_idx = kma_blocksizes_idx (power);

	tree_attach (&kma.used, &busy->t);

	/* If the block is too big, free the second half and look again.
	 * idx needs to be >0 because the first blocks can't be split */
	while (idx && idx > busy->size_idx) {
		idx--;
		kma_mark_free ((void *)freeb + blocksizes[idx], idx);
	}

	return freeb;
}




static u8 kma_init_done;


/* This is the main allocation function */
void *
kma_alloc (size_t size)
{
	u8 i = 0;
	u8 power;


	if (kma_init_done == 0)
		kpanic ("Memory subsystem not yet initialized!");


	/* The user should be using the pysical allocator */
	if (size > PAGE_SIZE)
		return NULL;

	power = num2power (size);

	i = kma_blocksizes_idx (power);

	while (i < BLOCKSIZES_SZ) {
		if (!list_empty (&kma.free[i].l))
			break;

		i++;
	}

	if (i == BLOCKSIZES_SZ) {
		kma_pool_expand_one ();
		i--;
	}

	return give_first_and_split_buddies (i, power);
}



/* Comparation function for the elements of the tree */
static s8
kma_busy_cmp (const struct tnode *a, const struct tnode *b)
{
	struct busy_block *u1 = containerof (a, struct busy_block, t);
	struct busy_block *u2 = containerof (b, struct busy_block, t);

	if (u1->addr < u2->addr)
		return 1;
	else if (u1->addr > u2->addr)
		return -1;
	else
		return 0;
}



void
kma_init (void)
{
	u8 i;
	
	list_init (&kma.pages);

	for (i = 0; i < BLOCKSIZES_SZ; i++)
		list_init (&kma.free[i].l);

	tree_init (&kma.used, kma_busy_cmp);


	kma_init_done = 1;


	busy_block_cache = kcache_create ("kma", sizeof (struct busy_block));
	kma_early_init_done = 1;

	init_kmalloc ();

	return;
}



