

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fs.h"


struct inode *root;



struct inode *
namei (const char *path)
{
	s16 len;
	struct inode *inode;

	/* TODO: add relative paths... */
	if (path == NULL || *path != '/')
		return NULL;

	inode = root;

	path++;

	while (path) {
		while (*path == '/')
			path++;

		if (*path == '\0')
			return inode;

		len = strlen2sep (path, '/');
		if (len == -1)
			len = strlen (path);
		if (len == 0)
			return inode;

		if (inode == root && strncmp (path, "..", len) == 0) {
			path += len;
			continue;
		}

		inode = inode->ops->lookup (inode, path, len);
		if (inode == NULL)
			return NULL;

		path += len;
	}


	return inode;
}



