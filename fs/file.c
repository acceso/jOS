

#include <stdint.h>
#include <string.h>

#include <lib/list.h>
#include <lib/math.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>

#include <kernel/task.h>

#include "fs.h"
#include "namei.h"



#define FILES_SLOT	10


static struct list_head files[FILES_SLOT];


static inline u8
file_hash (size_t inum)
{
	return inum % FILES_SLOT;
}


struct file *
file_get (const char *path)
{
	struct inode *inode;
	struct file *file;
	u8 hash_pos;

	inode = namei (path);
	if (inode == NULL)
		return NULL;

	hash_pos = file_hash (inode->num);

	list_foreach_entry (file, &files[hash_pos], l) {
		if (file->inode == inode)
			return file;
	}

	file = xkmalloc (sizeof (struct file));

	file->inode = inode;
	file->count = 0;

	list_add (&files[hash_pos], &file->l);

	return file;
}



size_t
read (s32 fd, void *buf, size_t count)
{
	struct inode *inode;
	struct bhead *block;
	size_t rbytes;
	off_t offset;
	u16 len;

	if (current->fd[fd]->pos > current->fd[fd]->file->inode->filesize)
		return 0;

	inode = current->fd[fd]->file->inode;
	rbytes = current->fd[fd]->pos;

	while (count) {
		block = inode->ops->block_read (inode, current->fd[fd]->pos);
		if (block == NULL)
			break;

		offset = current->fd[fd]->pos % inode->sb->blocksize;
		len = min (inode->sb->blocksize - offset, count);

		memcpy (buf, block->data + offset, len);

		current->fd[fd]->pos += len;
		count -= len;
		buf += len;
	}

	return current->fd[fd]->pos - rbytes;
}



void
init_files (void)
{
	u8 i;

	for (i = 0; i < FILES_SLOT; i++)
		list_init (&files[i]);

	return;
}



