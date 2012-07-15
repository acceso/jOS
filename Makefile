
#elf = elf32
elf = elf64

PAGE_OFFSET = 0xffff800000000000
#PAGE_OFFSET = 0x0


SOURCES = \
	boot/boot32.o boot/main.o \
	drivers/disk.o drivers/keyboard.o drivers/timer.o drivers/vga.o \
	kernel/acpi.o kernel/mm.o kernel/mm_kmalloc.o kernel/mm_phys.o \
		kernel/traps.o kernel/intr.o \
	include/stdio.o include/stdlib.o include/string.o \
	lib/kernel.o lib/mem.o \
	$()


CFLAGS = \
	-ffreestanding -nostdlib -nostdinc -nostartfiles -nodefaultlibs \
	-fno-stack-protector -mno-red-zone \
	-Wall -std=gnu99 -I include -I . -iquote . -pipe -m64 -mcmodel=large \
	-ggdb -O2 \
	-DK_PAGE_OFFSET=$(PAGE_OFFSET)
LDFLAGS = -N -dT linker.ld --defsym page_offset=$(PAGE_OFFSET) # -M
ASFLAGS = $(CFLAGS)



ifeq ($(elf),elf32)
	LDFLAGS += --oformat=elf32-i386 
else ifeq ($(elf),elf64)
	LDFLAGS += --oformat=elf64-x86-64
endif


all: $(SOURCES)
	ld $(LDFLAGS) -o jOS $(SOURCES)
	scripts/run.sh qemu

qemu: all

bochs: $(SOURCES)
	ld $(LDFLAGS) -o jOS $(SOURCES)
	scripts/run.sh bochs


clean:
	rm -f *.o jOS */*.o



