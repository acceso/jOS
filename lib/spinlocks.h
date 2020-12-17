#ifndef LIB_SPINLOCKS_H
#define LIB_SPINLOCKS_H



#include <stdint.h>
#include <lib/bitset.h>



typedef u64 spinlock_t;


#define SPINLOCK_LOCKED	1
#define SPINLOCK_UNLOCKED	0



static inline void spinlock_lock(spinlock_t *l)
{
	asm volatile(
	"1:\n\t"
		"lock bts $0, %[l]\n\t"
		"jc 1b\n\t"
		: [l] "+m"(*l)
		:
		: "cc" ); /* cc = condition code */

	return;
}



static inline void spinlock_unlock(spinlock_t *l)
{
	bitclear((u64 *)&l, 0);
}



static inline void spinlock_barrier(spinlock_t *l)
{
	spinlock_lock(l);
	spinlock_unlock(l);
}



/* TODO: WARNING!! bittest is not atomic! (yet) */
static inline u8 spinlock_locked(spinlock_t *l)
{
	return bittest(*l, 0);
}





#endif /* LIB_SPINLOCKS_H */

