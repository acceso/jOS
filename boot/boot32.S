
/*
 *
 */


#include "../inc/boot.h"


/* Maps a full pdpe with 1GB pages starting from (base << 30 & flags) */
.macro map_pdpe pdpe_ptr, base_and_flags
	movl \pdpe_ptr, %edi
	movl \base_and_flags, %eax
	movl $512, %ecx
1:
	movl %eax, (%edi)
	addl $0x40000000, %eax	# each page maps 1G
	addl $8, %edi		# next entry, 8 bits
	loop 1b
.endm



	.set MB_MAGIC, 0x1BADB002
	.set MB_FLAGS, \
		1<<0 /* page align */ |\
		1<<1 /* memory info */ # |\
		# 1<<5 /* memory map */

	.section .mboot

	.code32

	.align 4

mb_header:
	/* Grub magic */
	.long MB_MAGIC
	.long MB_FLAGS
	.long -(MB_MAGIC + MB_FLAGS)


	/* Space for the multiboot information pointer */
	.global mbi32
mbi32:
	.long 0x0



	.global _start
_start:
	cli

	/* Save multiboot information */
	movl %ebx, mbi32

	/* Make sure CR0.PG = 0. */
	mov %cr0, %eax
	btr $31, %eax
	movl %eax, %cr0

	/* Segmentation */
	lgdt gdt_ptr

	movw $K_DS, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs


	/* Identity mapping for userspace */
	movl $pdpu + 0x7, pml4
	map_pdpe $pdpu, $0x87

	/* Identity mapping for the kernel */
	movl $pdpk + 0x7, pml4 + 0x800
	map_pdpe $pdpk, $0x87


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
	ljmp $K_CS, $start64

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
	.lcomm pdpu, 0x1000
	.lcomm pdpk, 0x1000



