
#include <inc/boot.h>
#include <inc/types.h>
#include <lib/bitset.h>
#include <lib/kernel.h>
#include <lib/stdio.h>

#include "mm_kmalloc.h"
#include "intr.h"
#include "timer.h"
#include "traps.h"


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
	//lapic_write (APIC_EOI, 0);
	kprintf ("EOIdooo!\n");
}



static void
do_spurious (void)
{
	kprintf ("Spurious int!\n");
}


static void
lapic_init (void)
{

	lapic.id = (u8)((lapic_read (APIC_ID) >> 24) & 0xf);
	lapic.version = (u8)(lapic_read (APIC_VERSION) & 0xff);

	/* Spurious interrupt handler */
	idt_set_gate (SPURIOUS_INTR, (u64)&do_spurious, K_CS, GATE_INT);

	/* Spurious interrupt. software enable | interrupt vector */
	lapic_write (APIC_SIVR, (1<<8) | SPURIOUS_INTR);

	/* Set flat mode */
	lapic_write (APIC_DFR, (0xf << 28) | lapic_read (APIC_DFR));

	/* Set TPR to 0, unblocks all interrupts */
	lapic_write (APIC_TPR, 0); 

	/* Apic error local vector table reg, readable error on APIC_ESR */
	lapic_write (APIC_EVTE, APIC_E_INTR);


return;

	/* allow external interrupts and nmi */
	lapic_write (APIC_LI0VTE, 0x8700);
	lapic_write (APIC_LI1VTE, 0x400);

	return;
}




void
init_interrupts (void)
{
	lapic.base = (void *)__va (0xfee00000);

	/* Fields:
	 * 0x800: EN enable/disable
	 * 0x100: BSP: bootstrap cpu core */
	msr_write (IA32_APIC_BASE_MSR, 0x800 | 0x100 | (u64)__pa (lapic.base));


	lapic_init ();

	// Send_INIT_IPI(); // wake up Application Processors
	//ioapic_init ();

	kprintf ("LAPIC id=%p ver=%p enabled (%llp)\n", 
		lapic.id, lapic.version, __pa (lapic.base));
}




