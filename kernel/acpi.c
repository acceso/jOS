

/* 
 * The ACPI spec is HUGE! It'd take me too long to have 
 * a working implementation :(.
 * I'm just going to be adding code as necessary.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lib/kernel.h>
#include <lib/mem.h>
#include <sys/io.h>

#include <kernel/cpu.h>
#include <kernel/intr.h>

#include "acpi.h"




#define RSDP_SIG	0x2052545020445352 /* "RSDP PTR " */
#define RSDT_SIG	0x54445352 /* RSDT */

#define APIC_SIG	0x43495041 /* APIC */
#define FADT_SIG	0x50434146 /* FACP */
#define SSDT_SIG	0x54445353 /* SSDT */
#define HPET_SIG	0x54455048 /* HPET */

#define FACS_SIG	0x53434146 /* FACS */
#define DSDT_SIG	0x54445344 /* DSDT */


struct acpi_header {
	u32 sig;
	u32 l;
	u8 rev;
	u8 csum;
	char oemid[6];
	char oemtableid[8];
	u32 oemrev;
	char creatorid[4];
	u32 creatorrev;
	u32 entry;
} __attribute__((__packed__));



struct acpi_apic {
	u8 type;
	u8 l;
	union {
		struct { /* When type 0, lapic info */
			u8 cpuid;
			u8 lapicid;
			u32 flags0; /* Just one: enabled (bit 0) */
		} __attribute__((__packed__));
		struct { /* When type 1, ioapic info */
			u8 ioapicid;
			u8 reserved1;
			u32 ioapicaddr;
			u32 gsib;
		} __attribute__((__packed__));
		struct { /* When type 2, interrupt source override */
			u8 bus;
			u8 source;
			u32 gsi;
			u16 flags2;
		} __attribute__((__packed__));
		/* There are types for nmi, lapic nmi, address override
		 * and for sapic (ia64 apic) and x2apic */
	};
} __attribute__((__packed__));




extern struct _ioapic ioapic[];
extern struct _lapic lapic[];
extern struct _sys sys;

/* For now, just one lapic and ioapic are supported :( */
static void
acpi_walk_madt (void *t, s32 l)
{
	struct acpi_apic *a;
	u8 i;


	/* Default mappings between old 8259 and ioapic,
	 * the overrides will change this information */
	for (i = 0; i < 16; i++) {
		ioapic[0].pic[i].dest = i;
		ioapic[0].pic[i].edge = 1;
		ioapic[0].pic[i].active_high = 1;
	}

	sys.cpu[0].lapic = &lapic[0];
	lapic[0].base = (void *)__va (*(u32 *)(u64)(t + 0));


	if (*(u32 *)(t + 4) & 0x1)
		sys.has8259 = 1;


	/* 8 bytes consumed so far */
	l = l - 8;
	/* Points to "APIC Structure[0]" */
	a = (struct acpi_apic *)(t + 8);


	while (l > 0) {
		if (a->l == 0)
			break;

		switch (a->type) {
		case 0: /* Processor Local APIC */
			lapic[0].id = a->lapicid;
			sys.cpu[0].id = a->cpuid;

			break;
		case 1: /* I/O APIC */
			ioapic[0].id = a->ioapicid;
			ioapic[0].base = (void *)__va (a->ioapicaddr);

			break;
		case 2: /* Interrupt Source Override */
			if (a->source > 16 || a->gsi > 16)
				break;


			kprintf ("  PIC Redirect %d -> %d", 
				a->source, a->gsi);

			ioapic[0].pic[a->source].dest = a->gsi;


			/* 0b00 bus default (edge)
			 * 0b01 edge
			 * 0b11 level */
			if (((a->flags2 >> 2) & 0x3) == 0x3) {
				ioapic[0].pic[a->gsi].edge = 0;
				kprintf (" (level, ");
			} else
				kprintf (" (edge, ");


			/* 0b00 bus default (active low),
			 * 0b01 active high, 
			 * 0b11 active low */
			if ((a->flags2 & 0x3) != 0x1) {
				ioapic[0].pic[a->gsi].active_high = 0;
				kprintf ("low)\n");
			} else
				kprintf ("high)\n");


			/* Thats all folks :) */

			break;
		default:
			break;
		}

		l -= a->l;
		a = (struct acpi_apic *)((u64)a + a->l);
	}

	return;
}


static void
acpi_walk_facs (void *t)
{
	if (*(u32 *)t != FACS_SIG)
		return;

	/* etc... */
}


static void
acpi_walk_dsdt (void *t)
{
	if (*(u32 *)t != DSDT_SIG)
		return;

	/* etc... */
}

/* These are not supposed to change as the apic1 spec remains stable
 * and we're not going for newer vesions... 
 * The header is 36 bits so everything is shifted 36 bytes */
static void
acpi_walk_fadt (void *t)
{
	acpi_walk_facs (t);	/* bit 36 */
	acpi_walk_dsdt ((u8 *)t + 4); /* bit 40 */
}


static void
acpi_walk_ssdt (void *t)
{
}




struct rsdp {
	u64 sig;
	u8 csum;
	char oemid[6];
	u8 rev;
	u32 rsdt;
} __attribute__((__packed__));




static u64 ranges[][2] = {
	{ 0x9fc00, 0xa0000 }, /* EBDA Extended BIOS Data Area */
	{ 0xe0000, 0xfffff }, /* BIOS ROM */
	{ 0x00000, 0x00000 }
};


void *
acpi_search_table (u64 signature)
{
	u8 siglen = 64;
	u8 i = 0;
	u64 *p;


	/* Will fail if a 64 bit signature is something like:
	 * "\0\0\0\0etc." but afaik there are no such signatures.. */
	if ((signature & 0xffffffff) == signature)
		siglen = 32;

	do {
		if (ranges[i] == 0)
			break;

		for (p = (u64 *)ranges[i][0]; p < (u64 *)ranges[i][1];
			p += 2) {

			if (siglen == 64 && *p == signature)
				return (void *)p;

			if (siglen == 32 && *(u32 *)p == (u32)signature)
				return (void *)p;

		}

	} while (ranges[++i][0] != 0);


	return NULL;
}




void
init_acpi (void)
{
	struct rsdp *rsdpp = NULL;
	struct acpi_header *h;
	char oem[7];
	u32 *p;

	kprintf ("Parsing ACPI tables:\n");


	rsdpp = (struct rsdp *)acpi_search_table (RSDP_SIG);
	if (rsdpp == NULL || rsdpp->rsdt == 0)
		kpanic ("No valid ACPI tables.");


	if (acpi_csum (rsdpp, 20) != 0)
		kpanic ("Wrong checksum in rsdp\n");


	/* Is not null terminated.. */
	strncpy (rsdpp->oemid, oem, 6);
	oem[6] = '\0';

	kprintf ("  ACPI table v%d by %s\n", rsdpp->rev, oem);



	h = (struct acpi_header *)(u64)rsdpp->rsdt;
	if (h->sig != RSDT_SIG)
		kpanic ("Invalid RSDT in ACPI table\n");

	if (acpi_csum (h, h->l) != 0)
		kpanic ("Wrong checksum in rsdt\n");


	for (p = (u32 *)&h->entry; p < (u32 *)((u64)h + h->l); p++) {

		struct acpi_header *h2 = (struct acpi_header *)(u64)*p;


		if (acpi_csum (h2, h2->l) != 0)
			continue;

		/*kprintf ("%s\n", &h2->sig);*/

		switch (h2->sig) {
		case APIC_SIG:
			/* This is the mother of the lamb!!
			 * A bit confusing because the signature (APIC) 
			 * is different from the table name! (MADT) */
			acpi_walk_madt (&h2->entry,
				h2->l - sizeof (struct acpi_header) + 4
				);
			break;
		case FADT_SIG:
			acpi_walk_fadt (&h2->entry);
			break;
		case SSDT_SIG:
			acpi_walk_ssdt (&h2->entry);
			break;
		case HPET_SIG:
			break;
		default:
			/*kprintf ("------>%s\n", &h2->sig);*/
			break;
		}
	}

}




