
#ifndef KCPU_H
#define KCPU_H


#include <stdint.h>

#include <kernel/intr.h>




struct _cpu {
	u8 id;
	struct _lapic *lapic;
	u64 hz;
};


struct _sys {
	struct _cpu cpu[1];
	u64 busspeed;
	u8 has8259;
};


struct _gdt {
	u16 limit;
	u16 base1;
	u8 base2;
	u8 type;
	u8 limit2;
	u8 base3;
	u32 base4;
	u32 reserved;
} __attribute__((__packed__));







void init_cpu ();


#endif /* KCPU_H */

