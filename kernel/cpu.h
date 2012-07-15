
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
	u8 has8259;
};

#endif /* KCPU_H */

