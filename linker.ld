

/* OUTPUT_FORMAT(elf64-x86-64) */

ENTRY(_start)

phys = 0x100000;
virt = 0x100000;

SECTIONS
{
    .text virt : AT(phys)
    {
	code = .;
	*(.mboot)
	*(.text)
	*(.rodata*)
	. = ALIGN(4096);
    }

    .data : AT(phys + data - code)
    {
	data = .;
	*(.data)
	. = ALIGN(4096);
    }

    .bss : AT(phys + bss - code)
    {
	bss = .;
	*(.bss)
	*(COMMON)
    }

    /* end of kernel :) */
    eok = .;
    /*. = ASSERT(eok <= 0x123456, "Too big!") */
}



