

	.code64

	.text

	movabsq $(stack + STACKSIZE), %rsp

	pushq $0x0;
	pushq $0x0;

	.global start64
start64:
	/* movabsq forces a 8-byte displacement */
	movabsq $kmain, %rax
	callq *%rax

1:
	hlt
	jmp 1b


	.bss

	.align 4096

	.set STACKSIZE, 0x1000
	.lcomm stack, STACKSIZE


