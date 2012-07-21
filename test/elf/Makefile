

# Here are a few of sample ELF files to help me out to incrementally
# develop the ELF-loader part :)


CC = gcc
CFLAGS = -Wall

# Simple infinite loop, can't think of anything more minimal
# as there aren't syscalls yet:
elf1: elf1.S
	as -o elf1.o elf1.S
	ld -o elf1 elf1.o
	strip elf1

# next:
# - same stuff + exit syscall
# - previous + uninitialized global (bss)
# - same thing but initialized global
# - const global :)
# - static elf with libc
# - dynamic elf :(, this one will take time if ever happens....



clean:
	@rm -f *.o
	@for a in *.S ; do rm -f `basename $$a .S` ; done



