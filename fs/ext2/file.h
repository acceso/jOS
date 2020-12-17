
#ifndef FS_EXT2_FILE_H
#define FS_EXT2_FILE_H


#include <stdint.h>




struct bhead *ext2_bread(struct super *sb, size_t bnum);



#endif /* FS_EXT2_FILE_H */

