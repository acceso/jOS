
#include <stdint.h>
#include <string.h>

#include <lib/math.h>

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
		current->fds[fd]->pos = offset;
		break;
	case SEEK_CUR:
		current->fds[fd]->pos += offset;
		break;
	case SEEK_END:
		current->fds[fd]->inode->filesize += offset;
		break;
	default:
		return (off_t) -1;
	}

	return current->fds[fd]->pos;
}




size_t
read (s32 fd, void *buf, size_t count)
{
	struct inode *inode;
	struct bhead *block;
	size_t rbytes;
	off_t offset;
	u16 len;

	if (current->fds[fd]->pos > current->fds[fd]->inode->filesize)
		return 0;

	inode = current->fds[fd]->inode;
	rbytes = current->fds[fd]->pos;

	while (count) {
		block = inode->ops->block_read (inode, current->fds[fd]->pos);
		if (block == NULL)
			break;

		offset = current->fds[fd]->pos % inode->sb->blocksize;
		len = min (inode->sb->blocksize - offset, count);

		memcpy (buf, block->data + offset, len);

		current->fds[fd]->pos += len;
		count -= len;
		buf += len;
	}

	return current->fds[fd]->pos - rbytes;
}




s8
open (const char *path, u16 flags, u16 mode)
{
	u32 fd;
	
	for (fd = 0; fd < NFDS; fd++)
		if (current->fds[fd] == NULL)
			break;

	if (fd >= NFDS - 1)
		return -1;

	current->fds[fd] = file_get (path);
	if (current->fds[fd] == NULL)
		return -1;

	current->fds[fd]->mode = mode;
	current->fds[fd]->flags = flags;
	current->fds[fd]->pos = 0;

	return fd;
}




