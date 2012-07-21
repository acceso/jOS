

#include <stdint.h>

#include <mm/kmalloc.h>
#include <mm/mm.h>
#include <vm/pg.h>

#include "task.h"


static struct task *init;


struct task *
init_task (void)
{
	u8 i;

	init = xkmalloc (sizeof (struct task));

	init->state = TASK_RUNNING;

	for (i = 0; i < NFDS; i++)
		init->fds[i] = NULL;

	load_init_page_tables (init);

	/* TODO: open /dev/std{in,out,err} */

	return init;
}



