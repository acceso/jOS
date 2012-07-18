
#ifndef LIB_DEBUG_H
#define LIB_DEBUG_H

#include <stdint.h>


/* With this, we'll realize when we left code around :P */
#ifdef DEBUG


/* This is useful for bochs */


#define dbg_break()				\
	asm volatile ("xchg %bx, %bx\n")


#define dbg_break_cond(cond) do {		\
		if (cond)			\
			dbg_break ();		\
	} while (0)


/* For now, it'll be per source file
 * Maybe I could do something like:
   #ifndef DBGENABLED -> u8 dbgenabled; define DBGENABLED
   #else -> extern u8 dbgenabled; */
static u8 dbgenabled __attribute__((unused));


#define dbg_if_enabled() 			\
	dbg_break_cond (dbgenabled)


#define dbg_enable()				\
	dbgenabled = 1


#define dbg_enable_if(cond) do {		\
	if (cond)				\
		dbg_enable();			\
} while (0)


#define dbg_disable()				\
	dbgenabled = 0


#endif /* DEBUG */



#endif /* LIB_DEBUG_H */


