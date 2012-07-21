

.section .text


.globl _start
_start:
	movl $0x5, %eax
	/* Call the bochs debugger: */
	xchg %bx, %bx
loop:
	jmp loop




