


#include <lib/multiboot.h>

#include <inc/types.h>
#include <boot/traps.h>
#include <drivers/vga.h>
#include <lib/io.h>


u16
kstart (multiboot_info_t *mbi)
{
	int a;

	cls();
	puts("Booting jOS kernel...\n");


	init_exceptions ();

	/*a = 2 / 0;*/
	asm volatile ("int $0x03");
	asm volatile ("int $0x04");


	return 0;
}


