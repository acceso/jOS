

#include "vma.h"



void
init_mm (struct mm *mm, void *pml4)
{
	list_init (&mm->l);

	mm->pgd = pml4;

	return;
}



