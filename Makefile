

arch=x86
#arch=x86-64


SOURCES = boot.o main.o vga.o lib.o

CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-Wall -pedantic -ggdb -std=gnu99 -iquote . -O2 \
	-nostartfiles -nodefaultlibs
LDFLAGS=-N -Tlink.ld
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


all: $(SOURCES) link
	#/home/jose/_jOS/copy.sh
	/home/jose/wip/jOS/run.sh

clean:
	rm -f *.o jOS

link:
	ld $(LDFLAGS) -o jOS $(SOURCES)





