
#include <stdint.h>

#include <lib/list.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>

#include "inode.h"



struct inode *
iget (struct super *sb, size_t inum)
{
	struct inode *ino;

	list_foreach_entry (ino, &sb->icache, l) {
		if (ino->num == inum) {
			ino->count++;
			return ino;
		}
	}

	ino = xkmalloc (sizeof (struct inode));

	ino->num = inum;
	ino->count = 0;

	list_add (&sb->icache, &ino->l);

	return ino;
}



void
iput (struct inode *inode)
{
	list_del (&inode->l);
	kfree (inode);
}



size_t
bmap (struct inode *inode, off_t offset)
{
	return inode->ops->bmap (inode, offset);
}



