
.code64

.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set FLAGS,    1 | 1<<1  # this is the Multiboot 'flag' field

.align 4
.long MAGIC
.long FLAGS
.long -(MAGIC + FLAGS)


# initial kernel stack space
.set STACKSIZE, 0x4000			# 16k.
.comm stack, STACKSIZE, 32		# reserve 16k stack on a quadword boundary


.global loader
loader:
	mov $(stack + STACKSIZE), %esp	# set up the stack
	push %rax			# Multiboot magic number
	push %rbx			# Multiboot data structure

	call jmain			# call kernel proper

	cli

hang:
	hlt
	jmp hang



