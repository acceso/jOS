
#ifndef DRIVERS_CHAR_H
#define DRIVERS_CHAR_H


#include <stdint.h>

#include "device.h"


#define CMAJOR_HD	1

/* Set to the higher + 1 */
#define CMAJORMAX	2


struct cdevsw {
	void (*open)();
	void (*close)();
	void (*read)();
	void (*write)();
	void (*ioctl)();
	void (*mmap)();
	void (*segmap)();
	void (*poll)();
	void (*halt)();
};



void cdev_register_dev(struct cdevsw *cdev, u32 major);
void init_cdev(void);


#endif /* DRIVERS_CHAR_H */

