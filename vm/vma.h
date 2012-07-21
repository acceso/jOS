
#ifndef VM_VMA_H
#define VM_VMA_H



#include <stdint.h>

#include <fs/fs.h>



struct mm {
	struct list_head l; /* List of VMAs */
	u64 *pgd;
	u8 count;
	void *code_start;
	void *code_end;
	void *data_start;
	void *data_end;
	void *heap_start;
	void *heap_end;
	void *stack_start;
	u16 used_pages;
	/* args, env, ... */
};


#define VMA_READABLE		1<<0
#define VMA_WRITABLE		1<<1
#define VMA_EXECUTABLE		1<<2
#define VMA_SHARED		1<<3
#define VMA_CANGROW		1<<4
#define VMA_CANGROWDOWN		1<<5


struct vma {
	struct list_head l; /* Next vma */
	struct mm *mm;
	void *start;
	void *end;
	u8 flags;
	struct file *file;
	size_t offset;
	/* ops: open (when vma added to an address space), close, fault, 
	 * mkwrite, access. */
};




void init_mm (struct mm *mm, void *pml4);



#endif /* VM_VMA_H */

