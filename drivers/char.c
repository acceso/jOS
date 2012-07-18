


#include <stdint.h>

#include <lib/kernel.h>

#include "char.h"



static struct cdevsw *cdevsw[CMAJORMAX];





void
cdev_register_dev (struct cdevsw *cdev, u32 major)
{
	if (major > CMAJORMAX)
		kpanic ("CDEV: too big major number registered.");

	cdevsw[major] = cdev;
}



void
init_cdev (void)
{

}




