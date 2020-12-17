

#include <stdint.h>
#include <stdio.h>

#include <lib/kernel.h>
#include <lib/list.h>

#include <mm/kmalloc.h>

#include <vm/syscall.h>

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


struct inode *fs_mount(dev_t *dev, const char *dir)
{
	struct inode *inode;
	struct super *sb;
	u8 i;

	for (i = 0; i < FSN; i++) {
		if (fs[i] == NULL)
			return NULL;

		sb = fs[i]->prepare_mount(dev);
		if (sb == NULL)
			continue;

		break;
	}

	if (sb == NULL)
		kpanic("Can't mount root!");

	if (*dir == '/' && *(dir + 1) == '\0')
		return sb->ops->inode_read(sb, 2);

	// TODO: ....
	inode = namei(dir);
	if (inode == 0 || !S_ISDIR(inode->mode) || inode->covered)
		return NULL;

	return inode;
}



u8 fs_register(struct fs *fsn)
{
	static u8 id;

	if (id >= FSN)
		kpanic("Too many registered filesystems!");

	fs[id++] = fsn;

	return id;
}



s64 sys_read()
{
	return 0;
}



s64 sys_write(int fd, const void *buf, size_t count)
{
	/* There are _at least_ 3 problems with this function:
	 * - it should work with any other descriptor.
	 * - buffer should be copied to kernel space...
	 * - kprintf shouldn't be used as it uses formats and requires
	 *   a \0 terminated string
	 * and well, the terminal should be done...
	 * */
	if (fd < 1 || fd > 2)
		return 0;

	kprintf(buf);

	return 0;
}



s64 sys_open(const char *path, int flags, mode_t mode)
{
	return 0;
}



s64 sys_close()
{
	return 0;
}






void init_fs(dev_t *rdev)
{
	init_files();
	init_ext2();

	syscall_register(__NR_read, sys_read);
	syscall_register(__NR_write, sys_write);
	syscall_register(__NR_open, sys_open);
	syscall_register(__NR_close, sys_close);


	root = fs_mount(rdev, "/");
}



