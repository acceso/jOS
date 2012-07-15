

#include <stdio.h>




void
kpanic (char *str)
{
	if (str == NULL)
		str = "";

	puts ("\n\n");
	puts (str);

	puts ("\n\n\nOoooops!!! Habemus pete!\n\n"
		 "<Halted>\n");

	asm(
		"1:\n\t"
			"cli\n\t"
			"hlt\n\t"
			"jmp 1b\n"
	);
}



