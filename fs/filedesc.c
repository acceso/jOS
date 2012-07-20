

#include <stdint.h>

#include <kernel/task.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>

#include "fs.h"
#include "file.h"
#include "filedesc.h"



off_t
lseek (s32 fd, off_t offset, u8 origin)
{
	switch (origin) {
	case SEEK_SET:
		current->fd[fd]->pos = offset;
		break;
	case SEEK_CUR:
		current->fd[fd]->pos += offset;
		break;
	case SEEK_END:
		current->fd[fd]->file->inode->filesize += offset;
		break;
	default:
		return (off_t) -1;
	}

	return current->fd[fd]->pos;
}



s8
open (const char *path, u16 flags, u16 mode)
{
	struct file *file;
	u32 fd;
	
	file = file_get (path);
	if (file == NULL)
		return -1;

	for (fd = 0; fd < NOFILES; fd++)
		if (current->fd[fd] == NULL)
			break;

	current->fd[fd] = xkmalloc (sizeof (struct filedesc));

	current->fd[fd]->file = file;
	current->fd[fd]->mode = 0;
	current->fd[fd]->flags = 0;
	current->fd[fd]->pos = 0;

	return fd;
}




