
/* OUTPUT_ARCH(i386:x86-64) */

ENTRY(_start)

phys = 0x100000;


SECTIONS
{
    . = phys;

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

    /* this makes room for the kernel to use */
    .bss2 : AT(ADDR(.bss2) - page_offset)
    {
	bss2 = .;
	FILL(0)
	. += 0x200000;
    }


    /* end of kernel :) */
    eok = .;

    /*. = ASSERT(eok <= 0x400000, "linker.ld: Kernel too big!!!");*/
}



