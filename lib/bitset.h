
#ifndef BITSET_H
#define BITSET_H


#include <inc/types.h>


static void inline
bitset (u64 *word, u64 pos) 
{
	asm(	"bts %[bit], %[reg]\n"
		: [reg] "+m"(*word)
		: [bit] "ir" (pos));

}



static void inline
bitclear (u64 *word, u64 pos)
{
	asm(	"btr %[bit], %[reg]\n"
		: [reg] "+m"(*word)
		: [bit] "ir" (pos));
}



static void inline
bittoggle (u64 *word, u64 pos)
{
	asm(	"btc %[bit], %[reg]\n"
		: [reg] "+m"(*word)
		: [bit] "ir" (pos));
}



#endif /* BITSET_H */


