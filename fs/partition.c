

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <drivers/disk.h>

#include "partition.h"



static struct _partition part[4];



/* This is an entry as it is on disk */
typedef struct __attribute__((packed)) {
	u8 flags;
	u8 head_start; u8 sec_start; u8 cyl_start;
	u8 ptype;
	u8 head_end; u8 sec_end; u8 cyl_end;
	u32 lba_start;
	u32 lba_len;
} p_entry;


unsigned char first_sector[512];


#define MBR_PARTITIONS		0x1be

#define MAGIC_PARTITION_TYPE	0x7f


static void
read_pentry (u8 pnum)
{
	p_entry *p;

	/* First partition is partition 0 for us, not 1 */
	pnum--;

	p = (p_entry *)&first_sector[MBR_PARTITIONS + pnum * sizeof (p_entry)];

	part[pnum].flags = p->flags;
	part[pnum].type = p->ptype;
	part[pnum].base = p->lba_start;
	part[pnum].len = p->lba_len;

	/* This shouldn't probably be here but this is a hobbyist OS :) */
	if (p->ptype != MAGIC_PARTITION_TYPE)
		return;

	kprintf("Found root partition: sectors %d-%d\n", 
		part[pnum].base, part[pnum].len);
}



static hddrive root;



void
init_partition (void)
{
	root = ide_get_root ();

	ide_read_blocks (root, first_sector, 0, 1);

	/* This is the boot record signature, just a magic number */
	if (first_sector[510] != 0x55 || first_sector[511] != 0xaa) {
		kprintf("Invalid mbr on master drive! Skipping.\n");
		return;
	}


	read_pentry (1);
	read_pentry (2);
	read_pentry (3);
	read_pentry (4);

	//partition_write (get_root_partition (), 0, "J", 1);
}




/*************************************************************/


/* Things start to get interesting :), 
 * I believe no matter how well I try, I won't do it right.
 * So I give a total sucky solution and we'll improve with time.
 * We'll need block devices, char devices and many many more abstractions...
 *
 * BTW, it looks stupid that if we won't be supporting anything else
 * than the first master partition, we use a partition parameter...
 * anyway, let's see what we get :).
 */


partition
get_root_partition (void)
{
	return &part[0];
}



/* ouch ! Ok, from now on, everything is shameful. 
 * Please stop looking at the code :(.
 * I want to go _fast_ and leave higher level concepts for the future.
 * so please, don't be mean :(. 
 * BTW, we can stack several operations from an upper layer 
 * for a bigger write :D
 */
#define MAX_PARTITION_WRITE	(128*1024)
static char pbuffer[MAX_PARTITION_WRITE];
/* Writes "data" into partition p at offset bytes */
/* TODO: we suppose each sector is 512 bytes which not always is... */
void
partition_write (partition p, u64 offset, char *data, u64 len)
{
	u64 nsector;

	/* This is outside our partition! */
	if (offset > ((p->base + p->len) << 9))
		return;

	if (len > MAX_PARTITION_WRITE)
		return;

	/* By definition, if it's not out, it's in :) 
	 * As we don't have page cache nor anything similar (yet!),
	 * we read, we change, we write.
	 * 
	 * I am in pain for this :(, BUT, this is a learning experience.
	 * I'll be able (at least I hope) to tell the gains between PIO - DMA,
	 * cache vs no cache, etc. as I'll be adding them incrementally. */

	nsector = p->base + (offset >> 9);

	
	ide_read_blocks (root, pbuffer, nsector, len % 512 + 1);
	strncpy (pbuffer + offset % 512, data, len);
	ide_write_blocks (root, pbuffer, nsector, len % 512 + 1);
}




