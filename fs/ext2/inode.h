
#ifndef FS_EXT2_INODE_H
#define FS_EXT2_INODE_H




struct inode *ext2_inode_read(struct super *sb, u64 inum);
void ext2_inode_write(struct super *sb, struct inode *inode);




#endif /* FS_EXT2_INODE_H */

