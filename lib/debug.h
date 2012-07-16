
#ifndef DEBUG_H
#define DEBUG_H


#ifdef DEBUG

#define dbg_break()	\
	asm volatile ("xchg %bx, %bx\n")

#else

#define dbg_break()	do {} while (0) 

#endif /* DEBUG */



#endif /* DEBUG_H */


