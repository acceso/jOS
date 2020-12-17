

#include <stdint.h>

#include "fs.h"
#include "filedesc.h"
#include "namei.h"



#define NFILES	128


static struct file file_table[NFILES];



struct file *file_get(const char *path)
{
	u8 i = 0;

	for (; i < NFILES; i++) {
		if (file_table[i].count == 0)
			break;
	}

	file_table[i].inode = namei(path);
	if (file_table[i].inode == NULL)
		return NULL;

	file_table[i].count = 1;

	return &file_table[i];
}



void init_files(void)
{
	u8 i;

	for (i = 0; i < NFILES; i++)
		file_table[i].count = 0;

	return;
}



