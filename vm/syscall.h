
#ifndef VM_SYSCALL_H
#define VM_SYSCALL_H


#include <stdint.h>

#include "cpu.h"



/* Steps to add a syscall: 
 * - check the number is free.
 * - add: #define __NR_num N, to this file.
 * - include this file.
 * - call: syscall_register (__NR_num, your_handler);
 * your_handler can be defined as you want but max 6 parameters are accepted
 * and it must return a signed integer.
 * */


#define __NR_read			0
#define __NR_write			1
#define __NR_open			2
#define __NR_close			3
#define __NR_stat			4
#define __NR_fstat			5
#define __NR_lstat			6
#define __NR_poll			7

#define __NR_lseek			8
#define __NR_mmap			9
#define __NR_mprotect			10
#define __NR_munmap			11
#define __NR_brk			12
#define __NR_rt_sigaction		13
#define __NR_rt_sigprocmask		14
#define __NR_rt_sigreturn		15


// ...


#define __NR_syscall_num		16




extern void (*syscall_dispatch) (void);

s8 syscall_register (u16 num, void *handler);


void init_syscall (void);


#endif /* VM_SYSCALL_H */


