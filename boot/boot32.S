
#define _ASM
#include "../lib/kernel.h"
#define IA32_EFER 0x0c0000080



/* Maps a full pde with 2MB pages starting from (base << 21 & flags) */
.macro map_pde pde_ptr, base_and_flags
	movl \pde_ptr, %edi
	movl \base_and_flags, %eax
	movl $512, %ecx
1:
	movl %eax, (%edi)
	addl $0x200000, %eax	# each page maps 2M
	addl $8, %edi		# next entry, 8 bits
	loop 1b
.endm



	.set MB_MAGIC, 0x1BADB002
	.set MB_FLAGS, \
		1<<0 /* page align */ |\
		1<<1 /* memory info */ |\
		1<<16 /* a.out kludge */

	.section .mboot

	.code32

	.align 4


mb_header:
	/* Grub magic */
	.long MB_MAGIC
	.long MB_FLAGS
	.long -(MB_MAGIC + MB_FLAGS)
	/* These are for the a.out kludge: */
	.long mb_header
	.long mboot /* Start of .text */
	.long bss - K_PAGE_OFFSET /* End of data */
	.long eok - K_PAGE_OFFSET /* End of bss */
	.long _start /* Entry point */


	/* Space for the multiboot information pointer */
	.global mbi32
mbi32:
	.long 0x0



	.global _start
_start:
	cli

	/* Save multiboot information */
	movl %ebx, mbi32


	/* Segmentation */
	lgdt gdt_ptr

	movw $K_DS, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	/* Make sure paging is disabled */
	movl %cr0, %eax
	btr $31, %eax /* PG: Clear paging */
	movl %eax, %cr0

	movl %cr4, %eax
	bts $4, %eax /* PSE, Page Size Extensions */
	bts $5, %eax /* PAE, 64-bit page-translation table entries */
	bts $7, %eax /* PGE, Page Global enable */
	movl %eax, %cr4


	mov $IA32_EFER, %ecx
	rdmsr
	bts $8, %eax /* LME = 1. Enable long mode. */
	wrmsr


	/* TODO: map 1GB pages...it comes even chaper */
	/* 512 * 2MB -> enough for now ( pow(2,30), 1GB ) */
	map_pde $pml2, $0x87
	movl $pml2 + 0x7, pml3

#if 1
	/* 0x9f is like 0x87 but no caching, TODO: enable! */
	map_pde $pml2d, $0x9f + 0xc0000000
	movl $pml2d + 0x7, pml3 + 0x18
#endif
	movl $pml3 + 0x7, pml4
	movl $pml3 + 0x7, pml4 + 0x800

	/* Load page directory base register */
	movl $pml4, %eax
	movl %eax, %cr3



	movl %cr0, %eax
	bts $1, %eax  /* PE: Enable protected mode */
	bts $31, %eax /* PG: Paging enable */
	movl %eax, %cr0


	/* jump to a 64 bit segment */
	ljmp $K_CS, $start64


	.code64

start64:

	movw $K_SS, %ax
	movw %ax, %ss
	movabsq $(stack + STACKSIZE), %rsp

	pushq $0x0;
	pushq $0x0;

	/* movabsq forces a 8-byte displacement */
	movabsq $kmain, %rax
	callq *%rax

/* should be unreachable: */
1:
	//hlt
	jmp 1b



	.data

	.align 8

gdt_ptr:
	.word 48 - 1
	.long gdt - K_PAGE_OFFSET



	.bss

	.align 4096

	.lcomm pml4, 0x1000
	.lcomm pml3, 0x1000
	.lcomm pml2, 0x1000
#if 1
	.lcomm pml2d, 0x1000
#endif


