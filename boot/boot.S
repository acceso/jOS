
/*
 *
 */

	.set MB_MAGIC, 0x1BADB002
	.set MB_FLAGS, 1<<0 | 1<<1 # page align | memory info

	.section .mboot

	.code32

	.align 4

mb_header:
	/* Grub magic */
	.long MB_MAGIC
	.long MB_FLAGS
	.long -(MB_MAGIC + MB_FLAGS)


	.global _start
_start:
	cli

	/* Make sure CR0.PG = 0. */
	mov %cr0, %eax
	btr $31, %eax
	movl %eax, %cr0

	/* Segmentation */
	lgdt gdt_ptr

	/* Page tables */
	movl $pdp + 0x7, pml4
	movl $pd + 0x7, pdp
	movl $pt + 0x7, pd

	/* Map 512 entries */
	movl $pt, %edi
	movl $0x7, %eax # flags
	movl $512, %ecx
loop_pt:
	movl %eax, (%edi)
	addl $0x1000, %eax # each page maps 4kB
	addl $8, %edi # next entry
	loop loop_pt


	movl $pml4, %eax
	movl %eax, %cr3


	/* PAE, enable 64-bit page-translation table entries */
	movl %cr4, %eax
	bts $5, %eax
	movl %eax, %cr4

	/* IA32_EFER.LME = 1. Enable long mode. */
	mov $0x0c0000080, %ecx
	rdmsr
	bts $8, %eax
	wrmsr

	/* CR0.PG = 1. Enable paging */
	mov %cr0, %eax
	bts $31, %eax
	mov %eax, %cr0

	/* jump to a 64 bit segment */
	#pushl $0x8 # ???
	#movl $start64, %eax
	#jmp *%eax

	ljmp $0x8, $start64

/* should be unreachable: */
1:
	cli
	hlt
	jmp 1b



	.align 8

gdt_ptr:
	.word gdt_end - gdt - 1
	.long gdt

	.align 16
gdt:
	.quad 0x0000000000000000
k_cs:
	.quad 0x00AF98000000FFFF
k_ds:
	.quad 0x008F92000000FFFF
gdt_end:




	.bss

	.align 4096

	.lcomm pml4, 0x1000
	.lcomm pdp, 0x1000
	.lcomm pd, 0x1000
	.lcomm pt, 0x1000



