
// http://forum.osdev.org/viewtopic.php?f=1&t=18389


#include <stdint.h>
#include <stdio.h>
#include <lib/bitset.h>
#include <lib/cpu.h>
#include <lib/kernel.h>
#include <lib/mem.h>

#include "mm_kmalloc.h"
#include "intr.h"
#include "traps.h"



#define IOREGSEL	0x0
#define IOWIN		0x10


static struct {
	void *base;
	u8 id;
	u8 version;
} ioapic;



static inline void
ioapic_regsel (u8 reg)
{
	*(volatile u32 *)(ioapic.base + IOREGSEL) = reg; 
}



u32
ioapic_read (u8 reg)
{
	ioapic_regsel (reg);

	return *(volatile u32 *)(ioapic.base + IOWIN);
}



void
ioapic_write (u8 reg, u32 val)
{
	ioapic_regsel (reg);

	*(volatile u32 *)(ioapic.base + IOWIN) = val;
}



static inline u8
ioapic_redir_nint_to_reg (u8 n)
{
	return n * 2 + 0x10;
}


#if 0 // will needit..
static u64
ioapic_redir_read (u8 n)
{
	u64 v;

	u8 reg = ioapic_redir_nint_to_reg (n);

	v = ioapic_read (reg + 1);
	v <<= 32;
	v |= ioapic_read (reg);

	return v;
}
#endif


static void
ioapic_redir_write (u8 n, u64 val)
{
	u8 reg = ioapic_redir_nint_to_reg (n);

	ioapic_write (reg, val & 0xffffffff);
	ioapic_write (reg + 1, val >> 32);
}





static void
ioapic_init (void)
{
	u8 i;

	ioapic.base = (void *)__va (0xfec00000);


	ioapic.id = 0;
	ioapic_write (IOAPICID, ioapic.id);

	ioapic.version = (u8)(ioapic_read (IOAPICVER) & 0xff);

	for (i = 0; i < 24; i++)
		ioapic_redir_write (i,
			0xff00000000000000 + 0x800 + (i + 32));

	lapic_eoi ();

	kprintf ("IOAPIC id=%p ver=%p enabled (%llp)\n", 
		ioapic.id, ioapic.version, __pa (ioapic.base));
}







#define IA32_APIC_BASE_MSR	0x1b

static struct {
	void *base;
	u8 id;
	u8 version;
} lapic;



u32
lapic_read (u32 reg)
{
	return *(volatile u32 *)(lapic.base + reg);
}



void
lapic_write (u32 reg, u32 val)
{
	volatile u32 *addr = (volatile u32 *)(lapic.base + reg);

	*addr = val;
}



void
lapic_eoi (void)
{
	lapic_write (APIC_EOI, 0);
}



__isr__
do_spurious (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Spurious int!\n");

	lapic_eoi ();
	intr_exit ();
}



static void
lapic_init (void)
{
	lapic.base = (void *)__va (0xfee00000);

	/* Fields:
	 * 0x800: EN enable/disable
	 * 0x100: BSP: bootstrap cpu core */
	msr_write (IA32_APIC_BASE_MSR,
		0x800 | 0x100 | (u64)__pa (lapic.base));


	lapic.id = (u8)((lapic_read (APIC_ID) >> 24) & 0xf);
	lapic.version = (u8)(lapic_read (APIC_VERSION) & 0xff);

	/* Spurious interrupt handler */
	idt_set_gate (SPURIOUS_INTR, (u64)&do_spurious, K_CS, GATE_INT);

	/* Spurious interrupt. software enable | interrupt vector */
	lapic_write (APIC_SIVR, (1<<8) | SPURIOUS_INTR);

	/* Set flat mode */
	lapic_write (APIC_DFR, (0xf << 28));

	/* Logical destination register */
	lapic_write (APIC_LDR, 1 << 24);

	/* Set TPR to 0, unblocks all interrupts */
	lapic_write (APIC_TPR, 0); 

	/* Apic error local vector table reg,
	 * readable error on APIC_ESR */
	lapic_write (APIC_EVTE, APIC_E_INTR);

	/* allow external interrupts and nmi */
	lapic_write (APIC_LI0VTE, 0x8700);
	lapic_write (APIC_LI1VTE, 0x400);

	return;
}




void
init_interrupts (void)
{
	lapic_init ();

	ioapic_init ();

	kprintf ("LAPIC id=%p ver=%p enabled (%llp)\n", 
		lapic.id, lapic.version, __pa (lapic.base));
}



