


#include "multiboot.h"

#include <types.h>
#include <drivers/vga.h>


u16
kstart (multiboot_info_t *mbi)
{
	cls();
	puts("Booting jOS kernel...\n");

	return 0;
}


