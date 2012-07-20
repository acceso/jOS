
#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H


#include <stdint.h>

#include <vm/vma.h>



extern struct task *current;

#define NOFILES	32


/* Adapted from linux */
#define TASK_RUNNING		0


struct task {
	u8 state;

	struct filedesc *fd[NOFILES];

	struct mm *mm;
};




struct task *init_task (void);


#endif /* KERNEL_TASK_H */

