

.section .text


.globl _start
_start:
	movq $1, %rax;		// write( ...
	movq $1, %rdi;		// write(1, ...
	movq $str, %rsi;	// write(1, str, ...
	movq $14, %rdx;		// write(1, str, 14)
	syscall
	movq $60, %rax;		// exit( ...
	movq $0, %rdi;		// exit(0, ...
	syscall
loop:
	jmp loop


//.section .rodata
	str:    .asciz "Hola mundo!\n$ "


