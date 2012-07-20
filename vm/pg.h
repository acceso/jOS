
#ifndef VM_PG_H
#define VM_PG_H



#include <stdint.h>




static inline void
cr3_write (u64 val)
{
	asm volatile (
		"mov %0,%%cr3"
		: : "r" (val));
}





void load_init_page_tables (void);



#endif /* VM_PG_H */

