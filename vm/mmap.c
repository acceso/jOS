
#include <stdint.h>

#include <mm/mm.h>
#include <fs/filedesc.h>
#include <vm/pg.h>

#include "mmap.h"


/* TODO TODO TODO: this whole function is wrong. For now, I have to 
 * make sure nothing faults (as I have no page fault handler). */
void *
mmap (void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	size_t bytes;

	/* TODO: Already mapped, not sure what to do...
	 * probably should create a new map but it's not safe for now. */
	if (virt2phys (addr) != NULL)
		return NULL;

	/* TODO: allow mappings bigger than one page. */
	if (length > PAGE_SIZE)
		return NULL;

	/* TODO: allow anonymous mappings... */
	if ((flags & MAP_ANONYMOUS) == MAP_ANONYMOUS)
		return NULL;


	map_one_page (addr, addr);

	/* TODO: as stated above, this doesn't belong here: */
	lseek (fd, offset, SEEK_SET);

	bytes = read (fd, addr, length);

	while (bytes < PAGE_SIZE)
		*((u8 *)addr + bytes++) = '\0';


	return addr;
}



