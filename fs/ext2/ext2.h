
#ifndef FS_EXT2_EXT2_H
#define FS_EXT2_EXT2_H


#define E2PRIV(_p)	((struct ext2_super_ext2 *)_p)


struct ext2_bg_desc {
	u32 bg_block_bitmap; /* Block address of block usage bitmap, 1 used */
	u32 bg_inode_bitmap; /* Block address of inode usage bitmap, 1 used */
	u32 bg_inode_table; /* Starting block address of inode table [] */
	u16 bg_free_blocks_count; /* Number of unallocated blocks in group */
	u16 bg_free_inodes_count; /* Number of unallocated inodes in group */
	u16 bg_used_dirs_count; /* Number of directories in group */
	char pad[14];
} __attribute__((packed));



struct ext2_super_ext2 {
	u32 nblocks;
	size_t bgroup_base;
	u16 inode_size;
	u16 bg_desc_per_block;
	u32 inodes_per_group;
	u32 inodes_max;
};




#endif /* FS_EXT2_EXT2_H */

