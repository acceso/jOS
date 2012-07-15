
#ifndef SPINLOCKS_H
#define SPINLOCKS_H


#include <stdint.h>
#include <lib/bitset.h>


typedef u64 spin_lock_t;


#define SPIN_LOCK_LOCKED	1
#define SPIN_LOCK_UNLOCKED	0



static inline void
spin_lock_lock (spin_lock_t *l)
{
	asm volatile (
	"1:\n\t"
		"lock bts $0, %[l]\n\t"
		"jc 1b\n\t"
		: [l] "+m"(*l)
		:
		: "cc" ); /* cc = condition code */

	return; 
}



static inline void
spin_lock_unlock (spin_lock_t *l)
{
	bitclear ((u64 *)&l, 0);
}



static inline void
spin_lock_waiton (spin_lock_t *l)
{
	spin_lock_lock (l);
	spin_lock_unlock (l);
}



/* TODO: WARNING!! bittest is not atomic! (yet) */
static inline u8
spin_lock_locked (spin_lock_t *l)
{
	return bittest (*l, 0);
}





#endif /* SPINLOCKS_H */

