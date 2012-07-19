
#ifndef FS_FS_H
#define FS_FS_H


#include <stdint.h>

#include <lib/list.h>

#include <drivers/device.h>

/* Buffer needs super and we need bhead: */
struct inode;
struct super;

#include "buffer.h"



/* Number of different fs we support */
#define FSN	1



struct inode_ops {
	int (*create) (struct inode *, const char *, u8, u8);
	struct inode *(*lookup) (struct inode *, const char *, u8);
	int (*link) (struct inode *, struct inode *, const char *, u8);
	int (*unlink) (struct inode *, const char *, u8);
	int (*symlink) (struct inode *, const char *, u8, const char *);
	int (*mkdir) (struct inode *, const char *, u8, u8);
	int (*rmdir) (struct inode *, const char *, u8);
	int (*mknod) (struct inode *, const char *, u8, u8, dev_t);
	int (*rename) (struct inode *, const char *, u8, struct inode *, const char *, u8);
	int (*readlink) (struct inode *, char *, u16);
	int (*follow_link) (struct inode *, struct inode *, u8, u8);
	int (*bmap) (struct inode *, size_t);
	void (*truncate) (struct inode *);
	int (*permission) (struct inode *, u8);
};

struct inode {
	struct super *sb;
	size_t num;
	u16 mode;
	u8 nlinks;
	uid_t uid;
	gid_t gid;
	size_t filesize;
	size_t nblocks;
	time_t atime;
	time_t mtime;
	time_t ctime;
	u32 flags;
	u32 count;
	struct list_head l;
	struct super *covered;
	struct inode_ops *ops;
	void *priv;
};




#define SUPER_ERROR_FS	(1<<0)

struct super {
	dev_t dev;
	size_t blocksize;
	size_t blocksizephys;
	struct list_head bcache;
	struct list_head icache;
	struct super_ops *ops;
	u64 magic;
	time_t time;
	struct inode *mounted;
	u32 flags;
	void *priv;
};

struct super_ops {
	struct super *(*super_read) (dev_t *dev);
	void (*super_write) (struct super *sb);
	struct inode *(*inode_read) (struct super *sb, u64 inum);
	void (*inode_write) (struct super *sb, struct inode *inode);
	//void (*statfs) (struct super *, struct statfs *);
};



struct fs {
	char name[10];
	struct super *(*prepare_mount) (dev_t *dev);
	// read, stat, super_free, get_inode...
};




struct inode *fs_mount (dev_t *dev, const char *dir);
u8 fs_register (struct fs *fs);

void init_fs (dev_t *rdev);



#endif /* FS_FS_H */

