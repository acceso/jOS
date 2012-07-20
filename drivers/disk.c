
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lib/bitset.h>
#include <lib/debug.h>
#include <lib/kernel.h>
#include <sys/io.h>

#include <drivers/block.h>

#include <kernel/intr.h>
#include <kernel/timers.h>
#include <kernel/traps.h>

#include "device.h"
#include "disk.h"



#define IDE_CH1			0
#define IDE_CH2			1
#define IDE_MASTER		0
#define IDE_SLAVE		1

#define ide_datareg		0
#define ide_ereg		1 /* ro */
#define ide_features		1 /* wo */
#define ide_seccount0		2
#define ide_lba0		3
#define ide_lba1		4
#define ide_lba2		5
#define ide_devsel		6
#define ide_status		7 /* ro */
#define ide_cmdreg		7 /* wo */
#define ide_status_alt		0x206 /* ro */
#define ide_ctrl		0x206 /* wo */


#define IDECMD_READ_SECTORS		0x20
#define IDECMD_READ_SECTORS_EXT		0x24
#define IDECMD_WRITE_SECTORS		0x30
#define IDECMD_WRITE_SECTORS_EXT	0x34
#define IDECMD_IDENTIFY			0xec
#define IDECMD_FLUSH_CACHE		0xe7



static size_t ide_read_blocks (struct _drive *dev, size_t sector, void *data, size_t count);
static size_t ide_write_blocks (struct _drive *dev, size_t sector, void *data, size_t count);


static struct _channels channels[NCHANNELS];



static u8
ide_ready_wait (struct _drive *dev)
{
	u8 s;
	u16 times = 0;

/* TODO: temporal :) */
usleep (500);
return inb (dev->channel->iobase + ide_status);

	while (++times < 100) {
		s = inb (dev->channel->iobase + ide_status);

		// kprintf ("---->%d<---\n", s);

		if (bittest (s, 7)) /* Busy */
			usleep (500);
		else if (bittest (s, 0)) { /* Error */
			if (times < 5)
				continue;

			/* TODO: reset drive... */
			kprintf ("Ata %d error: %d\n", dev->devnum,
					inb (dev->channel->iobase + ide_ereg));
			kpanic ("Can't recover ata errors.");
		} else if (bittest (s, 3)) /* DRQ */
			usleep (500);
	}

	if (times == 0)
		/* TODO: reset and repeat? */
		kprintf ("Bogus ide command.\n");

	return s;
}



static u8
ide_cmd_read (struct _drive *dev, u16 reg)
{
	ide_ready_wait (dev);

	return inb (dev->channel->iobase + reg);
}



static u16
ide_cmd_read16 (struct _drive *dev, u16 reg)
{
	ide_ready_wait (dev);

	return inw (dev->channel->iobase + reg);
}



static void
ide_cmd_write (struct _drive *dev, u16 reg, u8 data)
{
	ide_ready_wait (dev);

	outb (dev->channel->iobase + reg, data);
}



static void
ide_cmd_write16 (struct _drive *dev, u16 reg, u16 data)
{
	ide_ready_wait (dev);

	outw (dev->channel->iobase + reg, data);
}



static u8
ide_dev_sel (struct _drive *dev)
{
	/* We've got the right one already selected */
	if (dev->channel->selected_drive == dev->devnum)
		return 0;

	/* see ata-2, 6.2.7:
	   0xa0 is 0b10100000, obsolete bits, bochs complaints...
	   1<<6 for LBA addressing,
	   1<<4 for device 1 (slave) or unset fordevice 0 (master) */
	ide_cmd_write (dev, ide_devsel, 0xa0 | (1 << 6) | (dev->devnum << 4));
	dev->channel->selected_drive = dev->devnum;
	
	return 1;
}


#if 0
static void
ide_reset (struct _drive *dev)
{
	/* Not yet done... */
	return;

	ide_cmd_write (dev, ide_ctrl, 0x4);
	usleep (10);
	ide_cmd_write (dev, ide_ctrl, 0x0);

	dev->channel->selected_drive = IDE_MASTER;

	msleep (150);
}
#endif


static void 
do_ide_dev (struct _drive *dev)
{
	/* Things to check:  
	 * - pci busmaster status byte to check the irq came from the disk
	 */
	ide_ready_wait (dev);
	/* - if it was a read op, read busmaster status reg
	 *   if bit 1 is set, save values and write a 2 to it
	 */
}



__isr__
do_ide1 (struct intr_frame r)
{
	intr_enter ();

	do_ide_dev (&channels[0].drive[channels[0].selected_drive]);

	lapic_eoi ();
	intr_exit ();
}



__isr__
do_ide2 (struct intr_frame r)
{
	intr_enter ();

	do_ide_dev (&channels[1].drive[channels[1].selected_drive]);

	lapic_eoi ();
	intr_exit ();
}



/* Changes something like: "1032547698" into "0123456789" */
void
ide_strncpy (char *dest, char *src, s16 l)
{
	char *p;
	u8 saved_l;

	*(dest + l--) = '\0';

	saved_l = (u8)l;

	while (l > 0) {
		*dest++ = *(src + 1);
		*dest++ = *src++;

		src++;

		l -= 2;
	};


	/* The drive pads the string with spaces */
	p = dest + saved_l;


	while (*p == ' ' || *p == 0)
		*p-- = '\0';

	return;
}



/* See p.127 from d1410r3b-ATA-ATAPI-6.pdf */
static u8
ide_identify (struct _drive *dev)
{
	/* Beware stack usage here, 
	 * shouldn't be a problem because this gets called at boot. */
	u16 b[256];
	u16 c;
	char *p;


	ide_dev_sel (dev);

	/* Send identify command */
	ide_cmd_write (dev, ide_cmdreg, IDECMD_IDENTIFY);

	u8 s = inb (dev->channel->iobase + ide_status_alt);
	if (s == 0 || bittest (s, 5) || bittest (s, 0))
		return 0;

	for (c = 0; c <= 255 ; c++)
		b[c] = ide_cmd_read16 (dev, ide_datareg);

	/* General configuration word,
	 * Bit 15 == 0 if the device conforms to the spec */
	if (b[0] & 0x800)
		return 0;

	/* Capabilities. 
	 * Bit 9 == 1 if the device supports LBA 
	 * Bit 8 == 1 if the device supports DMA */
	if ((b[49] & (0x200 | 0x100)) == 0)
		return 0;

	/* Bit 10 == 1 if the device supports LBA48 */
	if ((b[83] & (1<<10)) == 0)
		return 0;

	/* Misc info about the drive */
	ide_strncpy (dev->serial, (char *)(b + 10), HD_SERIAL_LEN);
	ide_strncpy (dev->fwrev, (char *)(b + 23), HD_FWREV_LEN);

	ide_strncpy (dev->model, (char *)(b + 27), HD_MODEL_LEN);
	/* Model sometimes comes with trailing spaces, this wipes them out: */
	p = dev->model + HD_MODEL_LEN - 1; /* set at '\0' */
	while (*--p == ' ')
		*p = '\0';


	/* Ata version, 0 or 0xffff means "not reported" */
	if (b[80] != 0x0 && b[80] != 0xffff)
		/* We can't use bitscan_right because a drive can support
		 * several different versions at the same time */
		dev->ata_version_max = bitscan_left ((u64)b[80]);

	dev->nsectors = 
		((u64)*(b + 103) << 48) 
		| ((u64)*(b + 102) << 32) 
		| (*(b + 101) << 16) 
		| *(b + 100);
	if (dev->nsectors == 0)
		dev->nsectors = (*(b + 61) << 16) | *(b + 60); 


	/* TODO: this should be calculated... */
	dev->blocksize = 512;


	return 1;
}




/***************************************************/


typedef struct __attribute__ ((__packed__)) {
	u8 flags;
	u8 head_start; u8 sec_start; u8 cyl_start;
	u8 ptype;
	u8 head_end; u8 sec_end; u8 cyl_end;
	u32 lba_start;
	u32 lba_len;
} p_entry;



static void
read_pentry (struct _drive *dev, u8 pnum, u8 *first_sector)
{
	p_entry *p;

	p = (p_entry *)&first_sector[0x1be + (pnum - 1) * sizeof (p_entry)];

	dev->part[pnum].flags = p->flags;
	dev->part[pnum].type = p->ptype;
	dev->part[pnum].base = p->lba_start;
	dev->part[pnum].len = p->lba_len;

	if (dev->part[pnum].len == 0)
		return;

	kprintf("    Found partition %d: %d KB\n", pnum,
		dev->part[pnum].len * dev->blocksize >> 10 );
}



static void
read_partitions (struct _drive *dev)
{
	u8 first_sector[512];

	ide_read_blocks (dev, 0, first_sector, 512);

	/* This is the boot record signature, just a magic number */
	if (first_sector[510] != 0x55 || first_sector[511] != 0xaa)
		return;

	/* Partition 0 is the full disk */
	dev->part[0].len = dev->nsectors * dev->blocksize;

	read_pentry (dev, 1, first_sector);
	read_pentry (dev, 2, first_sector);
	read_pentry (dev, 3, first_sector);
	read_pentry (dev, 4, first_sector);
}



static void
ide_init_dev (struct _channels *channel, u8 devnum)
{
	struct _drive *dev;
	
	dev = &channel->drive[devnum];

	dev->devnum = devnum;

	/* The definitive test */
	if (ide_identify (dev) == 0)
		return;

	kprintf ("  %s: %s %s %s (ata v%u), %d MB\n", 
		(dev->devnum == IDE_MASTER) ? "Master" : "Slave ", 
		dev->model, dev->serial, dev->fwrev, dev->ata_version_max,
		(dev->nsectors * dev->blocksize) >> 20 );

	read_partitions (dev);

	return;

}



static void
ide_init (u8 ch_num, u16 iobase, u8 irq, void *fp)
{
	struct _channels *channel = &channels[ch_num];

	channel->iobase = iobase;
	channel->irq = irq + 32;
	channel->selected_drive = 0xff;
	channel->devnum = ch_num;
	/* Has to be done before we can call ide_cmd_write */
	channel->drive[IDE_MASTER].channel = channel;
	channel->drive[IDE_SLAVE].channel = channel;


	/* If we can read the same thing we write, there is a controller. 
	 * It should be: DRQ_OFF but it won't if there are no drives! */
	ide_cmd_write (channels->drive, ide_seccount0, 0b10101010);
	ide_cmd_write (channels->drive, ide_lba0, 0b01010101);
	if (ide_cmd_read (channels->drive, ide_seccount0) != 0b10101010)
		return;
	if (ide_cmd_read (channels->drive, ide_lba0) != 0b01010101)
		return;

	intr_install_handler (irq, fp);

	kprintf ("IDE%d, detected controller at %p irq %u\n", 
		channel->devnum, iobase, irq);

	/* Slave should be checked first */
	ide_init_dev (channel, IDE_SLAVE);
	ide_init_dev (channel, IDE_MASTER);

}



/* ************************************** */



static inline void
ide_prepare_rwop (struct _drive *dev, u8 cmd, u64 sector, size_t count)
{
	ide_dev_sel (dev);

	ide_cmd_write (channels->drive, ide_seccount0, count >> 8);
	ide_cmd_write (channels->drive, ide_lba0, ((u64)sector >> 24) & 0xff);
	ide_cmd_write (channels->drive, ide_lba1, ((u64)sector >> 32) & 0xff);
	ide_cmd_write (channels->drive, ide_lba2, ((u64)sector >> 48) & 0xff);

	ide_cmd_write (channels->drive, ide_seccount0, count & 0xff);
	ide_cmd_write (channels->drive, ide_lba0, (u64)sector & 0xff);
	ide_cmd_write (channels->drive, ide_lba1, ((u64)sector >> 8) & 0xff);
	ide_cmd_write (channels->drive, ide_lba2, ((u64)sector >> 16) & 0xff);


	ide_cmd_write (channels->drive, ide_cmdreg, cmd);
}



struct _drive *
ide_get_dev (dev_t *device)
{
	switch (device->minor) {
		case 0 ... 15:
			return &channels[0].drive[IDE_MASTER];
		case 16 ... 31:
			return &channels[0].drive[IDE_SLAVE];
		case 32 ... 47:
			return &channels[1].drive[IDE_MASTER];
		case 48 ... 63:
			return &channels[1].drive[IDE_SLAVE];
	}

	return NULL;
}



static size_t
ide_partition_pos (struct _drive *dev, u32 minor, size_t pos)
{
	/* Even though we just support primary partitions (4 at most),
	 * dev->part[] has 16 elements. 
	 * It's statically allocated so it goes to the bss section 
	 * and partition->len is 0 if partition doesn't exists. */
	struct _partition *p = &dev->part[minor % 16];

	if (p->len == 0 || pos > p->len)
		return (unsigned)-1;

	return p->base + pos;
}



static size_t
ide_read_blocks (struct _drive *dev, size_t sector, void *data, size_t count)
{
	u16 *dptr = data;
	size_t seccount;

	/* data needs to be >= dev->blocksize */
	seccount = count / dev->blocksize;
	if (seccount == 0)
		return 0;

	ide_prepare_rwop (dev, IDECMD_READ_SECTORS_EXT, sector, seccount);

	/* We read 2 bytes on each read */
	count = seccount * dev->blocksize >> 1;

	/* This can be greatly optimized, no hurry */
	do {
		/* Note we don't check for a null pointer dereference */
		*dptr++ = ide_cmd_read16 (dev, ide_datareg);

		// kprintf ("%c", *((u8 *)dptr - 2));
		// kprintf ("%c", *((u8 *)dptr - 1));

		if (--count == 0)
			break;
	} while (1);

	return seccount * dev->blocksize;
}


/* Reads some blocks from an ide disk.
 * device: where to read from
 * pos: position relative to device
 * addr: pointer to store the data we read
 * count: size of addr
 */
static size_t
ide_read (dev_t *device, size_t pos, void *addr, size_t count)
{
	struct _drive *dev;
	size_t realpos;

	dev = ide_get_dev (device);
	if (dev == NULL)
		return 0;

	realpos = ide_partition_pos (dev,device->minor, pos);
	if (realpos == (unsigned)-1)
		return 0;

	return ide_read_blocks (dev, realpos, addr, count);
}



static size_t
ide_write_blocks (struct _drive *dev, size_t sector, void *data, size_t count)
{
	u16 *dptr = (u16 *)data;
	size_t seccount;

	seccount = count / dev->blocksize;
	if (seccount == 0)
		return 0;

	ide_prepare_rwop (dev, IDECMD_WRITE_SECTORS_EXT, sector, seccount);

	count = seccount * dev->blocksize >> 1;

	do {
		ide_cmd_write16 (dev, ide_datareg, *dptr++);

		ide_cmd_write (channels->drive, ide_cmdreg, IDECMD_FLUSH_CACHE);

		if (--count == 0)
			break;
	} while (1);

	return seccount * dev->blocksize;
}



/* Writes some blocks to disk */
static size_t
ide_write (dev_t *device, size_t pos, void *addr, size_t count)
{
	struct _drive *dev;
	size_t realpos;

	dev = ide_get_dev (device);
	if (dev == NULL)
		return 0;

	realpos = ide_partition_pos (dev,device->minor, pos);
	if (realpos == (unsigned)-1)
		return 0;

	return ide_write_blocks (dev, realpos, addr, count);
}



static size_t
ide_open (dev_t *device)
{
	struct _drive *dev;

	dev = ide_get_dev (device);
	if (dev == NULL)
		return 0;

	return dev->blocksize;
}




static struct bdevsw hd_ops = {
	.open = &ide_open,
	.close = NULL,
	.read = &ide_read,
	.write = &ide_write,
	.ioctl = NULL,
	.size = NULL,
	.halt = NULL
};



void
init_disks (void)
{
	ide_init (IDE_CH1, 0x1f0, 14, do_ide1);
	ide_init (IDE_CH2, 0x170, 15, do_ide2);
	//ide_init (IDE_CH3, 0x1e8, , do_ide3);
	//ide_init (IDE_CH4, 0x168, , do_ide4);

	bdev_register_dev (&hd_ops, BMAJOR_HD);

	return;
}



