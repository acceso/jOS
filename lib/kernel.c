

#include <lib/stdio.h>


void
kpanic (char *str)
{
	kprintf (str);

	kprintf ("\n\n\nOoooops!!! Habemus pete!\n\n"
		 "<Halted>\n");

	asm(
		"1:\n\t"
			"hlt\n\t"
			"jmp 1b\n"
	);
}



