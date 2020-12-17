
#ifndef VM_PG_H
#define VM_PG_H


#include <stdint.h>

#include <kernel/task.h>



#define pml4_offset(_addr)	((u64)(_addr) >> 39 & 0x1ff)
#define pdpe_offset(_addr)	((u64)(_addr) >> 30 & 0x1ff)
#define pde_offset(_addr)	((u64)(_addr) >> 21 & 0x1ff)

#define pte_addr(_pte)		((void *)((*(u64 *)(_pte)) & 0xffffffffff000L))


static inline void
cr3_write(u64 val)
{
	asm volatile(
		"mov %0,%%cr3"
		: : "r" (val));
}



void *virt2phys(const void *addr);
u8 map_one_page(void *phys, void *vaddr);




void load_init_page_tables(struct task *task);



#endif /* VM_PG_H */

