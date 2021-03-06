
#ifndef KERNEL_INTR_H
#define KERNEL_INTR_H


#include <stdint.h>


/* Vector for spurious interrupts: */
#define SPURIOUS_INTR		64
/* Interrupt vector on lapic error: */
#define APIC_E_INTR		65
/* Interrupt vector for every lapic tick: */
#define LAPIC_TIMER_INTR	66




struct _ioapic {
	void *base;
	u8 id;
	u8 version;
	struct {
		u8 dest;
		u8 edge;
		u8 active_high;
	} pic[16];
};

#define IOAPICID	0x0
#define IOAPICVER	0x1
#define IOAPICARB	0x2




#define APIC_ID		0x20
#define APIC_VERSION	0x30
#define APIC_TPR	0x80
#define APIC_APR	0x90
#define APIC_PPR	0xa0
#define APIC_EOI	0xb0
#define APIC_RRR	0xc0
#define APIC_LDR	0xd0
#define APIC_DFR	0xe0
#define APIC_SIVR	0xf0
#define APIC_ISR	0x100 /* to 0x170 */
#define APIC_TMR	0x180 /* to 0x1f0 */
#define APIC_IRR	0x200 /* to 0x270 */
#define APIC_ESR	0x280
#define APIC_ICRL	0x300
#define APIC_ICRH	0x310
#define APIC_TLVTE	0x320
#define APIC_ThLVTE	0x330
#define APIC_PCLVTE	0x340
#define APIC_LI0VTE	0x350
#define APIC_LI1VTE	0x360
#define APIC_EVTE	0x370
#define APIC_TICR	0x380
#define APIC_TCCR	0x390
#define APIC_TDCR	0x3e0
#define APIC_EAFR	0x400
#define APIC_EACR	0x410
#define APIC_SEOI	0x420
#define APIC_IER	0x480 /* to 0x4f0 */
#define APIC_LVT	0x500 /* to 0x530 */


#define LAPIC_ID	0x0
#define LAPIC_VERSION	0x10


struct _lapic {
	void *base;
	u8 id;
	u8 version;
};


extern struct _lapic lapic[1];


static inline u32 lapic_read(u32 reg)
{
	return *(volatile u32 *)(lapic[0].base + reg);
}



static inline void lapic_write(u32 reg, u32 val)
{
	volatile u32 *addr = (volatile u32 *)(lapic[0].base + reg);

	*addr = val;
}



static inline void lapic_eoi(void)
{
	lapic_write(APIC_EOI, 0);
}


static inline void interrupts_disable(void)
{
	asm volatile("cli\n\t");
}


static inline void interrupts_enable(void)
{
	asm volatile("sti\n\t");
}




void ioapic_redir_unmask(u8 n);

void init_interrupts(void);


#endif /* KERNEL_INTR_H */

