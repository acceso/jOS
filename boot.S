
.code32

.set MB_MAGIC,    0x1BADB002
.set MB_FLAGS,    1<<0 | 1<<1 # page align | memory info

.text

.align 4
mb_header:
.long MB_MAGIC
.long MB_FLAGS
.long -(MB_MAGIC + MB_FLAGS)

.long mb_header
.long code
.long bss
.long end
.long _start

.global _start
_start:
	cli

	movl $(stack + STACKSIZE), %esp
	pushl $0x0
	movl %esp, %ebp

	/* Clear flags */
#	pushl $0x0
#	popf

#	lgdt gdt_info

#	andl	$0xfffffff0, %esp


	/* disable paging */
#	mov %cr0, %eax
#	btc $31, %eax
#	movl %eax, %cr0

	/* enable 64-bit page-translation-table entries */
#	movl %cr4, %eax
#	bts $5, %eax
#	movl %eax, %cr4

	/* page tables */
#	movl pml4_base, %eax
#	movl %eax, %cr3

	/* Enable long mode */
#	mov $0x0c0000080, %ecx
#	rdmsr
#	bts $8, %eax
#	wrmsr
	
	/* enable paging */
/*	mov %cr0, %eax
	bts $31, %eax
	mov %eax, %cr0

	jmp start64

.code64
.align 8
start64:

	pushq %rbx
*/
	cli
	call kmain

	cli
hang:
	hlt
	jmp hang


gdt_info:
	.value	gdt_info - gdt - 1
	.long	gdt
	.long   0


gdt:
        .long   0
        .long   0

        /* GDT_B32DATA: 32 bit flat data descriptor */
        .value  0xFFFF  /* limit 0..15 */
        .value  0x0000  /* base 0..15 */
        .byte   0x0     /* base 16..23 */
        .byte   0x92    /* P = 1, read/write data */
        .byte   0xCF    /* G=1, B=1, Limit (16..19)=1111 */
        .byte   0x0     /* base 24..32 */

        /*
         * unused
         */
        .long   0
        .long   0


pml4_base:
	.long 0



.set STACKSIZE, 0x1000

.comm   stack, STACKSIZE





