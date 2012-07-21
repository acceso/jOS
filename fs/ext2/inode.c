

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lib/kernel.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>
#include <fs/buffer.h>
#include <fs/fs.h>
#include <fs/inode.h>
#include <fs/stat.h>

#include <drivers/device.h>

#include "ext2.h"
#include "file.h"



#define EXT2_BAD_INO		1
#define EXT2_ROOT_INO		2
#define EXT2_ACL_IDX_INO	3
#define EXT2_ACL_DATA_INO	4
#define EXT2_BOOT_LOADER_INO	5
#define EXT2_UNDEL_DIR_INO	6

#define EXT2_FT_UNKNOWN		0
#define EXT2_FT_REG_FILE	1
#define EXT2_FT_DIR		2
#define EXT2_FT_CHRDEV		3
#define EXT2_FT_BLKDEV		4
#define EXT2_FT_FIFO		5
#define EXT2_FT_SOCK		6
#define EXT2_FT_SYMLINK		7

#define EXT2_S_IFSOCK	0xc000
#define EXT2_S_IFLNK	0xa000
#define EXT2_S_IFREG	0x8000
#define EXT2_S_IFBLK	0x6000
#define EXT2_S_IFDIR	0x4000
#define EXT2_S_IFCHR	0x2000
#define EXT2_S_IFIFO	0x1000

#define EXT2_IRUSR	0x0100
#define EXT2_IWUSR	0x0080
#define EXT2_IXUSR	0x0040
#define EXT2_IRGRP	0x0020
#define EXT2_IWGRP	0x0010
#define EXT2_IXGRP	0x0008
#define EXT2_IROTH	0x0004
#define EXT2_IWOTH	0x0002
#define EXT2_IXOTH	0x0001

#define EXT2_NAME_LEN	256


struct inode_ops ext2_inode_ops;


struct ext2_bgroup_dt {
	u32 bg_block_bitmap; /* where the block bitmap is for this group, 
			      * 1 bit for each block in the group */
	u32 bg_inode_bitmap; /* where the inode bitmap is for this group, 
			      * 1 bit for each inode in the group */
	u32 bg_inote_table; /* where the inode table is for this group, 
			     * ext2_super.s_inodes_per_group inodes */
	u16 bg_free_blocks_count; /* free blocks in this group */
	u16 bg_free_inodes_count; /* free inodes in this group */
	u16 bg_used_dirs_count; /* inodes for directories in the group */
	u16 bg_pad;
	char bg_reserved[12];
} __attribute__ ((__packed__));




struct ext2_inode {
	u16 i_mode; /* See macros EXT2_S_IF* and EXT2_I* */
	u16 i_uid; /* File user id */
	u32 i_size; /* In rev0. size in bytes, in rev1 and later, and only for
		     * regular files, lower 32-bit size */
	u32 i_atime; /* Inode accessed time, seconds since jan 1st 1970 */
	u32 i_ctime; /* Inode creation time */
	u32 i_mtime; /* Last modification time */
	u32 i_dtime; /* Deletion time */
	u16 i_gid; /* group id */
	u16 i_links_count; /* dentries pointing to this inode, 0 means free */
	u32 i_blocks; /* Total number of 512-bytes reserved blocks */
	u32 i_flags; /* chattr flags, compression, etc. */
	u32 i_osdl; /* OS dependant value */
	u32 i_block[15]; /* 12 direct, 1 indirect, 1 double-indirect, 
			  * 1 triply-indirect block */
	u32 i_generation; /* File version */
	u32 i_file_acl; /* Block number with extended attributes */
	u32 i_dir_acl; /* For regular files, high 32 bits of the file size. */
	u32 i_faddr; /* Location of file fragment. Always 0... */
	char i_osd2[12]; /* */
} __attribute__ ((__packed__));



struct ext2_inode_priv {
	u32 blocks[15];
};



struct ext2_dirent {
	u32 inode;
	u16 rec_len;
	u8 name_len;
	u8 file_type;
	char name[EXT2_NAME_LEN];
} __attribute__ ((__packed__));




struct inode *
ext2_inode_read (struct super *sb, u64 inum)
{
	struct ext2_super_ext2 *sbpriv = sb->priv;
	size_t bgroup;
	struct bhead *buf;
	struct ext2_bg_desc *e2desc;
	size_t iindex;
	struct ext2_inode *e2ino;
	struct inode *inode;
	struct ext2_inode_priv *ipriv;
	u8 i;

	if (inum > sbpriv->inodes_max)
		return NULL;

	bgroup = (inum - 1) / sbpriv->inodes_per_group;

	/* there are: sb->blocksize / sizeof (struct ext2_bg_desc)
	   block descriptor entries on each block. */
	buf = ext2_bread (sb, sbpriv->bgroup_base
		+ bgroup / (sb->blocksize / sizeof (struct ext2_bg_desc)));

	e2desc = (struct ext2_bg_desc *) buf->data;

	/* TODO: I'm believe this code should be:
	 * e2desc += bgroup % (sb->blocksize / sizeof (struct ext2_bg_desc));
	 * but I'm not sure and in the small fs I use for testing it doesn't matter. */
	e2desc += bgroup;

#if 0
	/* Format of the debugfs tool, show_super_stats */
	kprintf ("\nGroup %u: block bitmap at %u, inode bitmap at %u, inode table at %u\n"
		 "%u free blocks, %u free inodes, %u used directories\n",
		 bgroup, e2desc->bg_block_bitmap, e2desc->bg_inode_bitmap, 
		 e2desc->bg_inode_table, e2desc->bg_free_blocks_count,
		 e2desc->bg_free_inodes_count, e2desc->bg_used_dirs_count);
#endif

	iindex = (inum - 1) % sbpriv->inodes_per_group;

	buf = ext2_bread (sb, e2desc->bg_inode_table + (iindex * sbpriv->inode_size) / sb->blocksize);

	e2ino = (struct ext2_inode *) buf->data;

	e2ino += (iindex % (sb->blocksize / sbpriv->inode_size));

	inode = iget (sb, inum);
	if (inode->count > 0)
		return inode;

	inode->count++;

	inode->sb = sb;
	inode->mode = e2ino->i_mode;
	inode->nlinks = e2ino->i_links_count;
	inode->uid = e2ino->i_uid;
	inode->gid = e2ino->i_gid;
	inode->filesize = e2ino->i_size;
	inode->nblocks = e2ino->i_blocks;
	inode->atime = e2ino->i_atime;
	inode->mtime = e2ino->i_mtime;
	inode->ctime = e2ino->i_ctime;
	inode->covered = NULL;
	inode->flags = e2ino->i_flags;
	inode->priv = xkmalloc (sizeof (struct ext2_inode_priv));

	ipriv = (struct ext2_inode_priv *)inode->priv;

	for (i = 0; i < 15; i++)
		ipriv->blocks[i] = e2ino->i_block[i];

// TODO: make this different for different file types? it can be done within 
// the functions themselves...
	inode->ops = &ext2_inode_ops;

	return inode;
}



static size_t 
ext2_bmap (struct inode *inode, off_t offset)
{
	struct ext2_inode_priv *ipriv = inode->priv;
	size_t nblock;
	struct bhead *block;

	nblock = offset / inode->sb->blocksize;

/*	kprintf ("%s inode %u block %u located in %u\n",
		__func__, inode->num, nblock, ipriv->blocks[nblock]); */

	if (nblock < 12)
		return ipriv->blocks[nblock];
	else if (nblock < 12 + (inode->sb->blocksize >> 2)) {
		if (ipriv->blocks[12] == 0)
			return 0;

		block = bread (inode->sb, ipriv->blocks[12]);

		return *(u32 *)(block->data + 4 * (nblock - 12));
	}
	/* TODO: as maths are too complex to me :), 
	 * still no double nor triply indirect block supported.
	 * This means a limit of blocksize * 268 bytes. 
	 * BTW, I could just return 0 instead of a panic but this way,
	 * I'll know when I need it. */
	else kpanic ("No double nor triply indirect block supported yet\n");

	return 0;

}



static struct bhead *
ext2_block_read (struct inode *inode, off_t offset)
{
	size_t blk;

	blk = bmap (inode, offset);
	if (blk == 0)
		return NULL;

	return bread (inode->sb, blk);
}



static struct inode *
ext2_lookup (struct inode *inode, const char *name, u8 len)
{
	size_t block = 0;
	size_t offset = 0;
	struct bhead *buffer;
	struct ext2_dirent *dirent;

	if (!S_ISDIR (inode->mode))
		return NULL;

	/* TODO: not tested if a dir spans two blocks... */
	while (offset <= inode->filesize) {
		buffer = ext2_block_read (inode, block);
		if (buffer == NULL)
			break;

		dirent = buffer->data;

		while ((void *)dirent < buffer->data + inode->sb->blocksize) {
			if (len == dirent->name_len &&
				strncmp (name, dirent->name, len) == 0)
				/* Err... this has to be adapted 
				 * to support mount points, something like:
				 * (inode->covered) ? inode->covered->sb 
				 *        : inode->sb */
				return ext2_inode_read (inode->sb, dirent->inode);

			dirent = (void *)dirent + dirent->rec_len;
		}


		offset += inode->sb->blocksize;
		block++;
	}


	return NULL;
}



struct inode_ops ext2_inode_ops = {
//	.create = NULL,
	.lookup = ext2_lookup,
//	.link = NULL,
//	.unlink = NULL,
//	.symlink = NULL,
//	.mkdir = NULL,
//	.rmdir = NULL,
//	.mknod = NULL,
//	.rename = NULL,
//	.readlink = NULL,
//	.follow_link = NULL,
	.bmap = ext2_bmap,
//	.truncate = NULL,
//	.permission = NULL,
	.block_read = ext2_block_read
};



