
#ifndef DRIVERS_DISK_H
#define DRIVERS_DISK_H


#include <stdint.h>


#define NCHANNELS 2


struct _channels;


struct _drive {
	u64 nsectors;

	u64 nread;
	u64 nwritten;

	u8 present;
	u8 ata_version_max;
	u16 blocksize;

	struct _channels *channel;

	/* Master / Slave */
	u8 devnum;

	/* +1 for the '\0' */
#define HD_SERIAL_LEN	(20 + 1)
	char serial[HD_SERIAL_LEN];
#define HD_FWREV_LEN	(8 + 1)
	char fwrev[HD_FWREV_LEN];
#define HD_MODEL_LEN	(40 + 1)
	char model[HD_MODEL_LEN];
};

typedef struct _drive * hddrive;


struct _channels {
	u16 iobase;
	u16 busmaster; /* For DMA... */
	u8 irq;
	u8 selected_drive;
	u8 devnum;

	struct _drive drive[2]; /* drive[0] master, drive[1] slave */

};



void init_disks (void);

void ide_read_blocks (struct _drive *dev, void *data, u64 sector, u16 count);
void ide_write_blocks (struct _drive *dev, void *data, u64 sector, u16 count);

hddrive ide_get_root(void);



#endif /* DRIVERS_DISK_H */

