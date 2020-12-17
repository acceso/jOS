

#include <stdint.h>

#include <lib/kernel.h>
#include <vm/cpu.h>

#include "elf.h"



s32 exec(const char *path)
{
	s64 entry;


	/* Todo: this is a bit barebones :P. */
	entry = elf_map(path);
	if (entry == -1)
		return -1;


	/* This is like: asm volatile("jmp *%0" : : "m" (entry));
	 * but it changes the stack and code segments: */
	usermode_jump(entry);


	return 0;
}




