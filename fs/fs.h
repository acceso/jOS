
#ifndef FS_FS_H
#define FS_FS_H


#include <stdint.h>

#include <lib/list.h>

#include <drivers/device.h>



struct inode;
struct super;


struct inode_ops {
	int (*create) (struct inode *, const char *, u8, u8);
	int (*lookup) (struct inode *, const char *, u8);
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
	dev_t dev;
	size_t num;
	u8 nlinks;
	uid_t uid;
	gid_t gid;
	size_t filesize;
	size_t nblocks;
	time_t atime;
	time_t mtime;
	time_t ctime;
	u64 blksize;
	struct super *covered;
	struct inode_ops ops;
};



#define SUPER_ERROR_FS	(1<<0)

struct super {
	dev_t dev;
	size_t blocksize;
	struct super_ops *ops;
	u64 magic;
	time_t time;
	struct inode *mounted;
	u32 flags;
};

struct super_ops {
	void (*inode_read) (struct inode *);
	void (*inode_write) (struct inode *);
	void (*write_super) (struct super *);
	//void (*statfs) (struct super *, struct statfs *);
};



struct fs {
	struct list_head l;
	char name[10];
	struct super *(*super_read) (dev_t *dev);
};




struct inode *fs_mount (dev_t *dev, const char *dir);
void fs_register (struct fs *fs);

void init_fs (dev_t *rdev);



#endif /* FS_FS_H */

