
#ifndef FS_FILE_H
#define FS_FILE_H

#include <stdint.h>



struct file *file_get (const char *path);
size_t read (s32 fd, void *buf, size_t count);
void init_files (void);


#endif /* FS_FILE_H */

