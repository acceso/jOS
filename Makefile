
#elf = elf32
elf = elf64

K_PAGE_OFFSET = 0xffff800000000000
#K_PAGE_OFFSET = 0x0

DEBUG = -DDEBUG

SOURCES = \
	boot/boot32.o kernel/main.o \
	drivers/block.o drivers/char.o drivers/disk.o drivers/keyboard.o drivers/lapictim.o \
	drivers/pit.o drivers/rtc.o drivers/vga.o \
	fs/fs.o fs/buffer.o fs/inode.o fs/namei.o \
		fs/ext2/dentry.o fs/ext2/file.o fs/ext2/inode.o fs/ext2/super.o \
	kernel/acpi.o kernel/cpu.o kernel/timers.o kernel/traps.o kernel/intr.o \
	mm/mm.o mm/kcache.o mm/kma.o mm/kmalloc.o mm/phys.o \
	include/stdio.o include/stdlib.o include/string.o include/time.o \
	lib/kernel.o lib/tree.o lib/mem.o \
	$()


CFLAGS = \
	-ffreestanding -nostdlib -nostdinc -nostartfiles -nodefaultlibs \
	-fno-stack-protector -mno-red-zone -mno-abm -msoft-float \
	-Wall -std=gnu99 -I include -I . -iquote . -pipe -m64 -mcmodel=large \
	-ggdb -O2 $(DEBUG) \
	-DK_PAGE_OFFSET=$(K_PAGE_OFFSET)
LDFLAGS = -N -dT linker.ld --defsym page_offset=$(K_PAGE_OFFSET) # -M
ASFLAGS = $(CFLAGS)



ifeq ($(elf),elf32)
	LDFLAGS += --oformat=elf32-i386 
else ifeq ($(elf),elf64)
	LDFLAGS += --oformat=elf64-x86-64
endif


all: $(SOURCES)
	ld $(LDFLAGS) -o jOS $(SOURCES)
	scripts/run.sh

qemu: all

bochs: $(SOURCES)
	ld $(LDFLAGS) -o jOS $(SOURCES)
	scripts/run.sh bochs


clean:
	rm -f *.o jOS */*.o */*/*.o
	cd ./test && make clean



