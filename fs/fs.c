

#include <stdint.h>
#include <stdio.h>

#include <lib/kernel.h>
#include <lib/list.h>

#include <mm/kmalloc.h>

#include <drivers/device.h>
#include <drivers/disk.h>

#include "ext2/super.h"
#include "file.h"
#include "fs.h"
#include "inode.h"
#include "namei.h"
#include "stat.h"



static struct fs *fs[FSN];
extern struct inode *root;


struct inode *
fs_mount (dev_t *dev, const char *dir)
{
	struct inode *inode;
	struct super *sb;
	u8 i;

	for (i = 0; i < FSN; i++) {
		if (fs[i] == NULL)
			return NULL;

		sb = fs[i]->prepare_mount (dev);
		if (sb == NULL)
			continue;

		break;
	}


	if (*dir == '/' && *(dir + 1) == '\0')
		return sb->ops->inode_read (sb, 2);

// TODO: ....
	inode = namei (dir);
	if (inode == 0 || !S_ISDIR(inode->mode) || inode->covered)
		return NULL;

	return inode;
}



u8
fs_register (struct fs *fsn)
{
	static u8 id;

	if (id >= FSN)
		kpanic ("Too many registered filesystems!");

	fs[id++] = fsn;

	return id;
}



void
init_fs (dev_t *rdev)
{
	init_files ();
	init_ext2 ();

	root = fs_mount (rdev, "/");

}



