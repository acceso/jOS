
#include <stdint.h>

#include <lib/list.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>

#include "inode.h"



struct inode *
iget (struct super *sb, size_t inum)
{
	struct inode *ino;

	list_foreach_entry (ino, &sb->icache, l) {
		if (ino->num == inum) {
			ino->count++;
			return ino;
		}
	}

	ino = kmalloc (sizeof (struct inode));
	if (ino == NULL)
		oom (__func__);

	ino->num = inum;
	ino->count = 0;

	list_add (&sb->icache, &ino->l);

	return ino;
}



void
iput (struct inode *inode)
{
	list_del (&inode->l);
	kfree (inode);
}



size_t
bmap (struct inode *inode, size_t offset)
{
	// esta función devuelve en que bloque dentro del inodo está el offset
	// ej. 
	// para / bloque 0 -> está en bloque 516
	// para /lost+found bloque 0 -> está en bloque 517
	// en este caso en lugar de bloque se nos da offset y sabiendo blocksize sale...
	return 0;
}



