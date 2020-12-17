
#ifndef FS_BUFFER_H
#define FS_BUFFER_H


#include <stdint.h>

#include <fs/fs.h>



struct bhead {
	struct list_head l;
	size_t bnum;
	void *data;
	u8 count;
	/* TODO: add r-time and w-time so blocks can be freed... */
};



struct bhead *getblk(struct super *sb, size_t block);
struct bhead *bread(struct super *sb, size_t block);


#endif /* FS_BUFFER_H */

