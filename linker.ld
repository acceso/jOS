
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)

/* start of kernel :). Beware: this is physical address but eok is not! */
sok = 0x100000;


SECTIONS
{
    . = sok;

    .mboot : AT(ADDR(.mboot))
    {
	mboot = .;
	*(.mboot)
	boot/boot*.o(.text)
	. = ALIGN(4096);
	boot/boot*.o(.bss)
    }

    . += page_offset;
    . = ALIGN(4096);

    .text : AT(ADDR(.text) - page_offset)
    {
	text = .;
	*(.text)
	*(.rodata*)
	. = ALIGN(4096);
    }

    .data : AT(ADDR(.data) - page_offset)
    {
	data = .;
	*(.data)
	. = ALIGN(4096);
    }

    .bss : AT(ADDR(.bss) - page_offset)
    {
	bss = .;
	*(.bss)
	*(COMMON)
    }


    /* end of kernel :) */
    eok = .;

    /*. = ASSERT(eok <= 0x400000, "linker.ld: Kernel too big!!!");*/
}



