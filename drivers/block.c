


#include <stdint.h>

#include <lib/kernel.h>

#include "block.h"
#include "device.h"



static struct bdevsw *bdevsw[BMAJORMAX];



size_t
bopen (dev_t *dev)
{
	if (dev->major >= BMAJORMAX)
		return 0;

	if (bdevsw[dev->major]->open == NULL)
		return 0;

	return bdevsw[dev->major]->open (dev);
}



size_t
breadu (dev_t *dev, size_t pos, void *addr, size_t count)
{
	if (dev->major >= BMAJORMAX)
		return 0;

	if (bdevsw[dev->major]->read == NULL)
		return 0;

	return bdevsw[dev->major]->read (dev, pos, addr, count);
}



size_t
bwrite (dev_t *dev, size_t pos, void *addr, size_t count)
{
	if (dev->major >= BMAJORMAX)
		return 0;

	if (bdevsw[dev->major]->write == NULL)
		return 0;

	return bdevsw[dev->major]->write (dev, pos, addr, count);
}



void
bdev_register_dev (struct bdevsw *bdev, u32 major)
{
	if (major > BMAJORMAX)
		kpanic ("BDEV: too big major number registered.");

	bdevsw[major] = bdev;
}



void
init_bdev (void)
{

}




