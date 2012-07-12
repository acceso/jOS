

/* OUTPUT_FORMAT(elf64-x86-64) */

ENTRY(_start)

phys = 0x100000;
virt = 0x100000;

SECTIONS
{
    .text virt : AT(phys)
    {
	text = .;
	*(.mboot)
	*(.text)
	*(.rodata*)
	. = ALIGN(4096);
    }

    .data : AT(phys + data - text)
    {
	data = .;
	*(.data)
	. = ALIGN(4096);
    }

    .bss : AT(phys + bss - text)
    {
	bss = .;
	*(.bss)
	*(COMMON)
    }

    /* this makes room for the kernel to use */
    .bss2 : AT(phys + bss2 - text)
    {
	bss2 = .;
	FILL(0)
	. += 0x200000;
    }


    /* end of kernel :) */
    eok = .;

    . = ASSERT(eok <= 0x400000, "linker.ld: Kernel too big!!!");
}



