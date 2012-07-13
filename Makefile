
elf = elf32
#elf = elf64

PAGE_OFFSET = 0xffff800000000000


SOURCES = boot/boot64.o \
	  kernel/mm.o kernel/mm_phys.o kernel/mm_kmalloc.o kernel/main.o \
	  lib/bitset.o lib/kernel.o lib/string.o lib/stdio.o lib/list.o \
	  drivers/vga.o \
	  kernel/traps.o

CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs -Wall -ggdb -std=gnu99 -iquote . \
	-O2 -pipe -m64 -mcmodel=large -DK_PAGE_OFFSET=$(PAGE_OFFSET)
LDFLAGS = -N -dT linker.ld --defsym page_offset=$(PAGE_OFFSET) # -M
ASFLAGS = $(CFLAGS)



ifeq ($(elf),elf32)
	LDFLAGS += --oformat=elf32-i386 
else ifeq ($(elf),elf64)
	LDFLAGS += --oformat=elf64-x86-64
endif


all: $(SOURCES)
	gcc -c -m32 -o boot/boot32_32.o boot/boot32.S
	objcopy -O elf64-x86-64 boot/boot32_32.o boot/boot32.o
	ld $(LDFLAGS) -o jOS boot/boot32.o $(SOURCES)
	/home/jose/wip/jOS/run.sh


clean:
	rm -f *.o jOS */*.o


