

#include <stdio.h>




void
kpanic (char *str)
{
	if (str == NULL)
		str = "";

	kputs ("\n\n");
	kputs (str);

	kputs ("\n\n\nOoooops!!! Habemus pete!\n\n"
		 "<Halted>\n");

	asm(
		"1:\n\t"
			"cli\n\t"
			"hlt\n\t"
			"jmp 1b\n"
	);
}



