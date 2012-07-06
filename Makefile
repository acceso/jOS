

#arch=x86
arch=x86-64


SOURCES = boot/boot64.o \
	  boot/kstart.o \
	  lib/lib.o \
	  drivers/vga.o

CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-Wall -pedantic -ggdb -std=gnu99 -iquote . -O2 \
	-nostartfiles -nodefaultlibs
LDFLAGS=-N -T linker.ld # -M
ASFLAGS=


ifeq ($(arch),x86)
	CFLAGS += -m32 -DARCH=X86
	LDFLAGS += -m elf_i386
	ASFLAGS += -m32
else ifeq ($(arch),x86-64)
	CFLAGS += -m64 -mcmodel=large -DARCH=X86-64
	LDFLAGS += -m elf_x86_64
	ASFLAGS += -m64
endif


all: $(SOURCES)
	gcc -c -m32 -o boot/boot32.o boot/boot.S
	objcopy -O elf64-x86-64 boot/boot32.o boot/boot.o
	ld $(LDFLAGS) -o jOS $(SOURCES) boot/boot.o 
	/home/jose/wip/jOS/run.sh

clean:
	rm -f *.o jOS boot/*.o kernel/*.o lib/*.o




