
#ifndef DRIVERS_BLOCK_H
#define DRIVERS_BLOCK_H


#include <stdint.h>

#include "char.h"
#include "device.h"



#define BMAJOR_HD	CMAJOR_HD

/* Set to the higher + 1 */
#define BMAJORMAX	2


struct bdevsw {
	/* I believe is a good idea to return the block size here :) */
	size_t (*open)(dev_t *device);
	void (*close)();
	size_t (*read)(dev_t *device, size_t pos, void *addr, size_t count);
	size_t (*write)(dev_t *device, size_t pos, void *addr, size_t count);
	void (*ioctl)();
	void (*size)();
	void (*halt)();
};



/* I don't want to forget checking the result value.
 * It's needed if a device doesn't support the operation. */
size_t bopen(dev_t *dev) WARN_UNUSED;
size_t breadu(dev_t *dev, size_t pos, void *addr, size_t count) WARN_UNUSED;
size_t bwrite(dev_t *dev, size_t pos, void *addr, size_t count) WARN_UNUSED;

void bdev_register_dev(struct bdevsw *bdev, u32 major);
void init_bdev(void);


#endif /* DRIVERS_BLOCK_H */

