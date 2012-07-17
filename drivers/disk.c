
/*
 * - make a good wait function
 * - resolve the spurious interrupt problem when initializing
 */



/* IDE driver operating in "compatibility" mode (ATA-1) 
 * Note: I'll skip stuff I know is ok in an emulator
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lib/bitset.h>
#include <lib/debug.h>
#include <lib/kernel.h>
#include <sys/io.h>

#include <kernel/intr.h>
#include <kernel/timers.h>
#include <kernel/traps.h>

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

#define selected_drive(dev)	(dev)->channel->selected_drive


#define IDECMD_READ_SECTORS		0x20
#define IDECMD_READ_SECTORS_EXT		0x24
#define IDECMD_WRITE_SECTORS		0x30
#define IDECMD_WRITE_SECTORS_EXT	0x34
#define IDECMD_IDENTIFY			0xec
#define IDECMD_FLUSH_CACHE		0xe7


static struct _channels channels[NCHANNELS];



static u8
ide_ready_wait (struct _drive *dev)
{
	u8 s;
	u8 times = 50;

/* TODO: This will do the same thing for now :) */
usleep (500);
return 1;

	while (times--) {
		s = inb (dev->channel->iobase + ide_status_alt);
		if (bittest (s, 7) == 0)
			break;
		else
			yield ();
	}

	/* Still busy or (ERR)or or (D)rive (F)ault*/
	if (times == 0 || bittest (s, 0) || bittest (s, 5)) {
		kprintf ("ATA Disk failure.\n");
		/* This is an illegal status value */
		return 0xff;
	}

	return s;
}



static u8
ide_cmd_read (struct _drive *dev, u16 reg)
{
	u8 s;
	u8 times = 50;

	while (times--) {
		s = ide_ready_wait (dev);
		if (bittest (s, 3) == 0)
			break;
	}

	if (s == 0xff)
		/* Note 0xff can be read from a reigster... 
		 * will have to be careful here */
		return 0xff;


	return inb (dev->channel->iobase + reg);
}



static u16
ide_cmd_read16 (struct _drive *dev, u16 reg)
{
	ide_ready_wait (dev);

	return inw (dev->channel->iobase + reg);
}



static void
ide_cmd_write16 (struct _drive *dev, u16 reg, u16 data)
{
	ide_ready_wait (dev);

	outw (dev->channel->iobase + reg, data);
}



static void
ide_cmd_write (struct _drive *dev, u16 reg, u8 data)
{
	if (ide_ready_wait (dev) == 0)
		return;

	outb (dev->channel->iobase + reg, data);
}



static u8
ide_dev_sel (struct _drive *dev)
{
	/* We've got the right one already selected */
	if (selected_drive (dev) == dev->devnum)
		return 0;

	/* see ata-2, 6.2.7:
	   set 1<<6 for LBA addressing,
	   set 1<<4 for device 1 (slave) or unset fordevice 0 (master) */
	ide_cmd_write (dev, ide_devsel, (1<<6) | (dev->devnum<<4) );
	dev->channel->selected_drive = dev->devnum;
	
	return 1;
}


#ifdef _notyet
static void
ide_reset (struct _drive *dev)
{
	/* Note: after a reset, the master drive is selected: */
	ide_cmd_write (dev, ide_ctrl, 0x4);
	usleep (10);
	ide_cmd_write (dev, ide_ctrl, 0x0);

	dev->channel->selected_drive = IDE_MASTER;

	msleep (100);
}
#endif



__isr__
do_ide (struct intr_frame r)
{
	intr_enter ();

	/* u8 s, e; */

	/* They will hapen even though using PIO, there is no point... */
	/* kprintf ("disk intr!\n"); */


	/* Things to check:  
	 * - pci busmaster status byte to check the irq came from the disk
	 * - read status reg to clear the interrupt flag
	 * - if err == 1, read error io port (ide_ereg)
	 * - if it was a read op, read busmaster status reg
	 *   if bit 1 is set, save values and write a 2 to it
	 */

	/* Don't have a "dev" struct pointer yet... TODO:
	s = ide_cmd_read (dev, ide_status);
	if (bittest (s, 7)) {
		e = ide_cmd_read (dev, ide_ereg);
		...
	}
	*/


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


	/* Send identify command */
	ide_cmd_write (dev, ide_cmdreg, IDECMD_IDENTIFY);


	for (c = 0; c < 256 ; c++)
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

	/* The second way makes gcc happy: */
	/* dev->nsectors = *(u32 *)(b + 100); */
	/* dev->nsectors = (*(b + 61) << 16) | *(b + 60); 
	 * but we'll be using lba48: */
	dev->nsectors = 
		((u64)*(b + 103) << 48) 
		| ((u64)*(b + 102) << 32) 
		| (*(b + 101) << 16) 
		| *(b + 100);


	/* TODO: this should be calculated... */
	dev->blocksize = 512;


	return 1;
}



static void
ide_init_dev (struct _channels *channel, u8 devnum)
{
	u8 s;
	struct _drive *dev;
	
	dev = &channel->drive[devnum];

	dev->devnum = devnum;


	ide_dev_sel (dev);

	/* If the status is illegal, there's no drive */
	s = ide_cmd_read (dev, ide_status);
	if (s == 0xff)
		return;

	/* The definitive test */
	if (ide_identify (dev) == 0)
		return;


	kprintf ("  %s: %s %s %s (ata v%u), %d MB\n", 
		(dev->devnum == IDE_MASTER) ? "Master" : "Slave ", 
		dev->model, dev->serial, dev->fwrev, dev->ata_version_max,
		(dev->nsectors * dev->blocksize) >> 20 );


//	ide_reset (dev);

	return;



}



static void
ide_init (u8 ch_num, u16 iobase, u8 irq)
{
	struct _channels *channel = &channels[ch_num];

	channel->iobase = iobase;
	channel->irq = irq + 32;
	channel->selected_drive = 0xff;
	channel->devnum = ch_num;
	/* Has to be done before we can call ide_cmd_write */
	channel->drive[IDE_MASTER].channel = channel;
	channel->drive[IDE_SLAVE].channel = channel;

	/* If we can read the same thing we write, 
	 * we have a controller. */
	ide_cmd_write (channels->drive, ide_seccount0, 0b10101010);
	if (ide_cmd_read (channels->drive, ide_seccount0) != 0b10101010)
		return;


	intr_install_handler (irq, (u64)&do_ide);


	/* TODO: don't know if we have enough evidence yet... */
	kprintf ("IDE%d, detected controller at %p irq %u\n", 
		channel->devnum, iobase, irq);

	/* Slave should be checked first */
	ide_init_dev (channel, IDE_SLAVE);
	ide_init_dev (channel, IDE_MASTER);


}



void
init_disks (void)
{
	ide_init (IDE_CH1, 0x1f0, 14);
	ide_init (IDE_CH2, 0x170, 15);

	//ide_write_blocks (&channels[0].drive[IDE_MASTER], b, 1, 1);
	//ide_read_blocks (&channels[0].drive[IDE_MASTER], b, 1, 1);

	return;
}




/* ************************************** */



static inline void
ide_prepare_rwop(struct _drive *dev, u64 sector, u16 count, u8 cmd)
{
	ide_dev_sel (dev);

	ide_cmd_write (channels->drive, ide_seccount0, count >> 8);
	ide_cmd_write (channels->drive, ide_lba0, (u64)sector & 0xff);
	ide_cmd_write (channels->drive, ide_lba1, ((u64)sector >> 8) & 0xff);
	ide_cmd_write (channels->drive, ide_lba2, ((u64)sector >> 16) & 0xff);

/*	ide_cmd_write (channels->drive, ide_seccount0, count & 0xff);
	ide_cmd_write (channels->drive, ide_lba0, ((u64)sector >> 24) & 0xff);
	ide_cmd_write (channels->drive, ide_lba1, ((u64)sector >> 32) & 0xff);
	ide_cmd_write (channels->drive, ide_lba2, ((u64)sector >> 48) & 0xff);
*/

	ide_cmd_write (channels->drive, ide_cmdreg, cmd);


	ide_ready_wait (dev);

}



/* Reads some blocks from an ide disk.
 * dev: where to read from
 * data: pointer to store the data we read
 * sector: position on dev
 * count: sector cont
 */
void
ide_read_blocks (struct _drive *dev, void *data, u64 sector, u16 count)
{
	u16 *dptr = data;

	/* Ok, although I detect lba48, I can't make it work,
	 * 512 * pow(2,24) ought to be enough for anybody :) */
	ide_prepare_rwop(dev, sector, count, IDECMD_READ_SECTORS);


	u16 rbytes = dev->blocksize * count;

	/* This can be greatly optimized, no hurry */
	do {
		/* Note we don't check for a null pointer dereference */
		*dptr++ = ide_cmd_read16 (dev, ide_datareg);

		ide_cmd_write (channels->drive, ide_cmdreg, IDECMD_FLUSH_CACHE);

		rbytes -= 2;

	} while (rbytes);

}



/* Writes some blocks to disk */
void
ide_write_blocks (struct _drive *dev, void *data, u64 sector, u16 count)
{
	u64 remb;
	u16 *dptr = (u16 *)data;

	ide_prepare_rwop(dev, sector, count, IDECMD_WRITE_SECTORS);

	/* 2 bytes get written on each operation,
	 * the loop has to run half of the time */
	remb = dev->blocksize * count >> 1;

	do {
		ide_cmd_write16 (dev, ide_datareg, *dptr++);

		ide_cmd_write (channels->drive, ide_cmdreg, IDECMD_FLUSH_CACHE);

	} while (--remb);


}




