
#ifndef FS_INODE_H
#define FS_INODE_H


#include <stdint.h>

#include "fs.h"



struct inode *iget(struct super *sb, size_t inum);
void iput(struct inode *inode);
size_t bmap(struct inode *inode, off_t offset);



#endif /* FS_INODE_H */

