
.code64

.text

.global start64
start64:
	/* TODO: poner a cero? o en bss.. */
	movq $(stack + STACKSIZE), %rsp

	pushq $0x0;
	pushq $0x0;

	movq %rsp, %rbp

	pushq %rbx

	call kstart

1:
	hlt
	jmp 1b


.set STACKSIZE, 0x1000

.lcomm stack, STACKSIZE


