

#arch=x86
arch=x86-64


SOURCES = boot/boot64.o boot/kstart.o \
	  lib/lib.o lib/io.o \
	  drivers/vga.o \
	  boot/traps.o

CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-Wall -pedantic -ggdb -std=gnu99 -iquote . -O2 \
	-nostartfiles -nodefaultlibs -pipe
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
	gcc -c -m32 -o boot/boot32_32.o boot/boot32.S
	objcopy -O elf64-x86-64 boot/boot32_32.o boot/boot32.o
	ld $(LDFLAGS) -o jOS $(SOURCES) boot/boot32.o 
	/home/jose/wip/jOS/run.sh

clean:
	rm -f *.o jOS */*.o




