

#include <stdint.h>

#include <mm/kmalloc.h>
#include <mm/mm.h>
#include <vm/pg.h>
#include <vm/syscall.h>

#include "task.h"


static struct task *init;



void
kernel_idle (void)
{
	while (1)
		;
}



void
sys_exit (u64 code)
{
	asm volatile ("sti\n\t");
	kernel_idle();
}



struct task *
init_task (void)
{
	u8 i;

	syscall_register (__NR_exit, sys_exit);

	init = xkmalloc (sizeof (struct task));

	init->state = TASK_RUNNING;

	for (i = 0; i < NFDS; i++)
		init->fds[i] = NULL;

	load_init_page_tables (init);

	/* TODO: open /dev/std{in,out,err} */

	return init;
}



