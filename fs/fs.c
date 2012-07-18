
/* 
 * TODO: use a lsab for inodes?
 */

#include <stdint.h>

#include <lib/list.h>

#include <drivers/device.h>
#include <drivers/disk.h>

#include <mm/kmalloc.h>

#include "fs.h"
#include "ext2/super.h"


static struct inode *root;

static struct list_head fslist;



struct inode *
fs_mount (dev_t *dev, const char *dir)
{
	struct super *sb;
	struct fs *fs;
	struct inode *inode;

	list_foreach_entry (fs, &fslist, l) {
		sb = fs->super_read (dev);
		if (sb != NULL)
			break;

		return NULL;
	}

	inode = kmalloc (sizeof (struct inode));

	inode->dev.major = dev->major;
	inode->dev.minor = dev->minor;
//	inode->num = sb->root inode...
//	inode->covered = sb;

	return inode;
}


void
fs_register (struct fs *fs)
{
	list_add (&fslist, &fs->l);
}



void
init_fs (dev_t *rdev)
{
	init_ext2 ();

	root = fs_mount (rdev, "/");
}



