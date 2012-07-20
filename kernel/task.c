

#include <stdint.h>

#include <mm/kmalloc.h>
#include <mm/mm.h>

#include "task.h"


static struct task *init;

struct task *
init_task (void)
{
	u8 i;

	init = xkmalloc (sizeof (struct task));

	init->state = TASK_RUNNING;

	for (i = 0; i < NOFILES; i++)
		init->fd[i] = NULL;

	return init;
}



