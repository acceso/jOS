
#ifndef FS_FILEDESC_H
#define FS_FILEDESC_H


/* See /usr/include/asm-generic/fcntl.h for more */
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002


#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2


off_t lseek(s32 fd, off_t offset, u8 origin);
size_t read(s32 fd, void *buf, size_t count);
s8 open(const char *path, u16 flags, u16 mode);


#endif /* FS_FILEDESC_H */

