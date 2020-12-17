
#ifndef TEST_MEM_COMMON_H
#define TEST_MEM_COMMON_H


#include <mm/mm.h>

#include "test.h"



/* Redefinitions of kernel functions */


int posix_memalign(void **memptr, size_t alignment, size_t size);

void *get_pages(u32 order)
{
	void *p;
	s32 r;

	r = posix_memalign(&p, PAGE_SIZE, PAGE_SIZE * (1 << order));
	if (r != 0) {
		printf("Error allocating memory\n");
		exit(1);
	}

	return p;
}



void *get_one_page(void)
{
	return get_pages(0);
}



void free_page(void *addr)
{
	free(addr);
}



void free_pages(u64 *addr)
{
	free(addr);
}






/* Redefinitions of kernel functions with an '_' prepended because
 * they add some functionality. */


void *kma_alloc(size_t size);

void *_kma_alloc(size_t size)
{
	void *p;

	p = kma_alloc(size);

	/* I comment it for the test to pass,
	 * it's easier than making the test smarter. */
	/* printf ("Alloc: %p\n", p); */
	printf("Alloc: \n");

	return p;
}



void kma_free(void *addr);

void _kma_free(void *addr)
{
	/* printf ("Free: %p\n", addr); */
	printf("Free: \n");

	kma_free(addr);
}



#endif /* TEST_MEM_COMMON_H */

