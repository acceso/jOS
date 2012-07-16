

#include <stdint.h>
#include <stdio.h>

#include <lib/bitset.h>
#include <lib/cpu.h>
#include <lib/kernel.h>
#include <lib/mem.h>
#include <sys/io.h>
#include <mm/kmalloc.h>

#include "intr.h"
#include "traps.h"



#define IOREGSEL	0x0
#define IOWIN		0x10


struct _ioapic ioapic[1];



static inline void
ioapic_regsel (u8 reg)
{
	*(volatile u32 *)(ioapic[0].base + IOREGSEL) = reg; 
}



u32
ioapic_read (u8 reg)
{
	ioapic_regsel (reg);

	return *(volatile u32 *)(ioapic[0].base + IOWIN);
}



void
ioapic_write (u8 reg, u32 val)
{
	ioapic_regsel (reg);

	*(volatile u32 *)(ioapic[0].base + IOWIN) = val;
}



static inline u8
ioapic_redir_nint_to_reg (u8 n)
{
	return n * 2 + 0x10;
}


static u64
ioapic_redir_read (u8 n)
{
	u64 v;

	u8 reg = ioapic_redir_nint_to_reg (ioapic[0].pic[n].dest);

	v = ioapic_read (reg + 1);
	v <<= 32;
	v |= ioapic_read (reg);

	return v;
}



static void
ioapic_redir_write (u8 n, u64 val)
{
	if (ioapic[0].pic[n].dest > 16)
		return;

	/* Might be redirections: */
	n = ioapic_redir_nint_to_reg (ioapic[0].pic[n].dest);

	ioapic_write (n, val & 0xffffffff);
	ioapic_write (n + 1, val >> 32);
}



static void
ioapic_redir (u8 n, u64 val)
{
	/* This leaves the interrupt masked,
	 * will be unmasked by drivers */
	val |= (1 << 16);

	/* Bit 11,
	 * 0 -> physical mode, just the lapic id
	 * 1 -> logical mode, set of processors */
	val |= 0x800;

	/* Because bit11 == 1, this is the set of processors 
	 * which will recieve the interrupt: */
	val |= 0xff00000000000000;


	/* Level */
	if (ioapic[0].pic[n].edge == 0)
		val |= (1 << 15);

	/* Interrupt input pin polarity, 0=high active, 1=low active */
	if (ioapic[0].pic[n].active_high == 0)
		val |= (1 << 13);



	ioapic_redir_write (n, val);
}



void
ioapic_redir_unmask (u8 n)
{
	n = ioapic[0].pic[n].dest;


/*	if ((ioapic_redir_read (n) >> 13 & 1) == 1)
		kprintf ("%d----> low active, ", n);
	else
		kprintf ("%d----> high active, ", n);

	if ((ioapic_redir_read (n) >> 15 & 1) == 1)
		kprintf ("level sensitive\n");
	else
		kprintf ("edge sensitive\n");
*/

	ioapic_redir_write (n, ioapic_redir_read (n) & ~(1 << 16));
}





static void
ioapic_init (void)
{
	u8 i;

	ioapic_write (IOAPICID, ioapic[0].id);

	ioapic[0].version = (u8)(ioapic_read (IOAPICVER) & 0xff);

	for (i = 0; i < 16; i++)
		ioapic_redir (i, i + 32);

	lapic_eoi ();

	kprintf ("IOAPIC id=%p ver=%p enabled (%llp)\n", 
		ioapic[0].id, ioapic[0].version, __pa (ioapic[0].base));
}







#define IA32_APIC_BASE_MSR	0x1b

struct _lapic lapic[1];





__isr__
do_lapic_err (struct intr_frame r)
{
	intr_enter ();

	kprintf ("Lapic internal error.\n");

	lapic_eoi ();
	intr_exit ();
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
	if (lapic[0].base == 0)
		kpanic ("Local apic not found");

	/* Fields:
	 * 0x800: EN enable/disable
	 * 0x100: BSP: bootstrap cpu core */
	msr_write (IA32_APIC_BASE_MSR,
		0x800 | 0x100 | (u64)__pa (lapic[0].base));


	lapic[0].id = (u8)((lapic_read (APIC_ID) >> 24) & 0xf);
	lapic[0].version = (u8)(lapic_read (APIC_VERSION) & 0xff);

	/* Spurious interrupt handler */
	intr_install_handler (SPURIOUS_INTR, (u64)&do_spurious);

	/* Spurious interrupt. software enable | interrupt vector */
	lapic_write (APIC_SIVR, (1<<8) | SPURIOUS_INTR);

	/* Set flat mode */
	lapic_write (APIC_DFR, (0xf << 28));

	/* Logical destination register */
	lapic_write (APIC_LDR, 1 << 24);

	/* Set TPR to 0, unblocks all interrupts */
	lapic_write (APIC_TPR, 0); 

	intr_install_handler (APIC_E_INTR, (u64)&do_lapic_err);
	/* Apic error local vector table reg,
	 * readable error on APIC_ESR */
	lapic_write (APIC_EVTE, APIC_E_INTR);

	/* allow external interrupts and nmi */
	lapic_write (APIC_LI0VTE, 0x8700);
	lapic_write (APIC_LI1VTE, 0x400);

	return;
}



static void
pic_disable (void)
{
	/* We're going to use the lapic.
	 * this disables the pic: */
	outb (0xa1, 0xff);
	outb (0x21, 0xff);
}


void
init_interrupts (void)
{
	pic_disable ();

	lapic_init ();

	ioapic_init ();

	kprintf ("LAPIC id=%p ver=%p enabled (%llp)\n", 
		lapic[0].id, lapic[0].version, __pa (lapic[0].base));
}



