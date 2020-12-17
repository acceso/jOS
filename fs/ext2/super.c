

#include <stdio.h>

#include <mm/mm.h>
#include <mm/kmalloc.h>
#include <fs/fs.h>

#include <drivers/block.h>
#include <drivers/device.h>

#include "ext2.h"
#include "inode.h"



#define EXT2_SUPER_MAGIC	0xEF53

#define EXT2_VALID_FS		1
#define EXT2_ERROR_FS		2

#define EXT2_ERRORS_CONTINUE	1
#define EXT2_ERRORS_RO		2
#define EXT2_ERRORS_PANIC	3

#define EXT2_OS_LINUX		0
#define EXT2_OS_HURD		1
#define EXT2_OS_MASIX		2
#define EXT2_OS_FREEBSD		3
#define EXT2_OS_LITES		4

#define EXT2_GOOD_OLD_REV	0
#define EXT2_DYNAMIC_REV	1

#define EXT2_GOOD_OLD_INODE_SIZE	128


struct super_ops ext2_ops;

struct ext2_super {
	u32 s_inodes_count; /* Total number of inodes */
	u32 s_blocks_count; /* Total number of blocks */
	u32 s_r_blocks_count; /* Blocks reserved for the super user */
	u32 s_free_blocks_count; /* Total number of free blocks */
	u32 s_free_inodes_count; /* Total number of free inodes */
	u32 s_first_data_block; /* Block num of the superblock,
				   1 if blocks == 1 KB, 0 otherwise */
	u32 s_log_block_size; /* block size = 1024 << s_log_block_size */
	s32 s_log_frag_size; /* frag size: if > 0 -> 1024 << s_log_frag_size
				else 1024 >> -s_log_frag_size */
	u32 s_blocks_per_group; /* # of blocks per group */
	u32 s_frags_per_group; /* # of fragments per group */
	u32 s_inodes_per_group; /* # of inodes per group  */
	u32 s_mtime; /* Last time it was mounted */
	u32 s_wtime; /* Time of the last write */
	u16 s_mnt_count; /* # of mounts since last fsck */
	u16 s_max_mnt_count; /* # of mounts before fsck */
	u16 s_magic; /* EXT2_SUPER_MAGIC */
	u16 s_state; /* EXT2_ERROR_FS when mounted, EXT2_VALID_FS if not */
	u16 s_errors; /* What to do on error: EXT2_ERRORS_...
			 continue, mount ro or cause a kernel panic */
	u16 s_minor_rev_level; /* Revision level */
	u32 s_lastcheck; /* Time of last fs check */
	u32 s_checkinterval; /* Interval between fs checks */
	u32 s_creator_os; /* EXT2_OS... linux, hurd, etc */
	u32 s_rev_level; /* EXT2_GOOD_OLD_REV, EXT2_DYNAMIC_REV */
	u16 s_def_resuid; /* Default uid for reserved blocks (0) */
	u16 s_def_resgid; /* Default gid for reserved blocks (0) */

	/* EXT2_DYNAMIC_REV */
	u32 s_first_ino; /* First usable inode. */
	u16 s_inode_size; /* Size of struct inode, EXT2_GOOD_OLD_INODE_SIZE
			   * or 1<<s_inode_size */
	u16 s_block_group_nr; /* Block group nr hosting this superblock */
	u32 s_feature_compat; /* bitmask of compatible features */
	u32 s_feature_incompat; /* bitmask of incompatible features */
	u32 s_feature_ro_compat; /* mount r-o if any is unsupported */
	char s_uuid[16]; /* Unique id */
	char s_volume_name[16]; /* Label */
	char s_last_mounted[64]; /* Dir where last mounted */
	u32 s_algo_bitmap; /* compression methods */
} __attribute__((__packed__));



static struct super *ext2_super_read(dev_t *dev)
{
	struct ext2_super *e2sb;
	struct super *super;
	char buf[512];
	size_t blocksize;
	struct ext2_super_ext2 *e2sbpriv;

	blocksize = bopen(dev);
	if (blocksize == 0)
		return NULL;

	/* With this math blocksize has to be <= 1024 */
	if (breadu(dev, 1024 / blocksize, buf, 512) == 0)
		return NULL;

	e2sb = (struct ext2_super *)buf;

	if (e2sb->s_magic != EXT2_SUPER_MAGIC)
		return NULL;

	super = xkmalloc(sizeof(struct super));

	super->dev.major = dev->major;
	super->dev.minor = dev->minor;
	super->blocksize = 1024 << e2sb->s_log_block_size;
	super->blocksizephys = blocksize;
	list_init(&super->bcache);
	list_init(&super->icache);
	super->ops = &ext2_ops;
	super->magic = EXT2_SUPER_MAGIC;
	/* TODO... */
	// super->time =
	super->flags = 0;
	if (e2sb->s_state != EXT2_VALID_FS)
		super->flags |= SUPER_ERROR_FS;

	super->priv = xkmalloc(sizeof(struct ext2_super_ext2));

	e2sbpriv = super->priv;

	/* TODO: one gives 7 and the other 8 for my test fs, I should
	   properly round them up */
	// e2sbpriv->nblocks = e2sb->s_inodes_count / e2sb->s_inodes_per_group;
	e2sbpriv->nblocks = 1 + (e2sb->s_blocks_count / e2sb->s_blocks_per_group);
	e2sbpriv->bgroup_base = 2; /* This table is just after the superblock */
	e2sbpriv->inode_size = e2sb->s_inode_size;
	e2sbpriv->inodes_per_group = e2sb->s_inodes_per_group;
	e2sbpriv->inodes_max = e2sb->s_inodes_count;
	e2sbpriv->bg_desc_per_block = blocksize / sizeof(struct ext2_super_ext2);

	return super;
}



struct super_ops ext2_ops = {
	.super_read = ext2_super_read,
	.inode_read = ext2_inode_read
};



static struct super *ext2_prepare_mount(dev_t *dev)
{
	return ext2_super_read(dev);
}



static struct fs ext2 = {
	.name = "ext2",

	.prepare_mount = ext2_prepare_mount
};



void init_ext2(void)
{
	fs_register(&ext2);
}



