
	.code64

	.text

	.global start64
start64:
	movq $(stack + STACKSIZE), %rsp

	pushq $0x0;

	call kmain

1:
	hlt
	jmp 1b

	.bss

	.align 4096

	.set STACKSIZE, 0x1000
	.lcomm stack, STACKSIZE


