
#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H


#include <stdint.h>

#include <vm/vma.h>



extern struct task *current;

#define NFDS	32


#define TASK_RUNNING		0


struct task {
	u8 state;

	struct file *fds[NFDS];

	struct mm *mm;
};




void kernel_idle(void);

struct task *init_task(void);


#endif /* KERNEL_TASK_H */


