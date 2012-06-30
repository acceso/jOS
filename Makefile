.SUFFIXES: .asm

jos: 
	as --64 -o kstart.o kstart.S 
	gcc -ffreestanding -nostdlib -c -o kmain.o kmain.c -nostartfiles -nodefaultlibs -mcmodel=large 
	ld -T link.ld -o jOS kstart.o kmain.o


