
#ifndef BITSET_H
#define BITSET_H


#include <inc/types.h>


static void inline
bitset (u64 *word, u64 pos) 
{
	asm volatile (
		"bts %[bit], %[reg]\n"
		: [reg] "+m"(*word)
		: [bit] "ir" (pos)
		: "cc" ); /* cc = condition code */

}



static void inline
bitclear (u64 *word, u64 pos)
{
	asm volatile (
		"btr %[bit], %[reg]\n"
		: [reg] "+m"(*word)
		: [bit] "ir" (pos)
		: "cc" );
}



static void inline
bittoggle (u64 *word, u64 pos)
{
	asm volatile (
		"btc %[bit], %[reg]\n"
		: [reg] "+m"(*word)
		: [bit] "ir" (pos)
		: "cc" );
}


static u8 inline
bittest (u64 word, u8 pos)
{
	while (pos) {
		word >>= 1;
		--pos;
	}

	return word & 1;
/* TODO: doesn't work (yet) :)
        asm (	"bt %[bit], %[reg]\n"
		"jc 1f\n"
		"xor %%ax, %%ax\n"
		"inc %%ax\n"
		"1:\n"
		: [reg] "+m"(word)
		: [bit] "ir" (pos));
*/
}




void *align_to (void *a, int p);


#endif /* BITSET_H */


