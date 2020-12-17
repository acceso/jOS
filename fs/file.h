
#ifndef FS_FILE_H
#define FS_FILE_H

#include <stdint.h>

#include "fs.h"


struct file *file_get(const char *path);
void init_files(void);


#endif /* FS_FILE_H */

