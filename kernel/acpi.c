

/* 
 * The ACPI spec is HUGE! It'd take me too long to have 
 * a working implementation :(.
 * I'm just going to be adding code as necessary.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <lib/kernel.h>



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
			u32 flags; /* Just one: enabled (bit 0) */
		};
		struct { /* When type 1, ioapic info */
			u8 ioapicid;
			u8 reserved;
			u32 ioapicaddr;
			u32 ioapic_base;
		};
		struct { /* When type 2, interrupt source override */
			u8 bus;
			u8 source;
			u32 gsi;
			u16 flags;
		};
		/* There are types for nmi, lapic nmi, address override
		 * and for sapic (ia64 apic) and x2apic */
	};
};


static void
acpi_walk_madt (struct acpi_apic *t, s32 l)
{
	kprintf ("-->%d<--\n", t->type);
	kprintf ("-->%d<--\n", t->l);
	return;

	while (l > 0) {

		switch (t->type) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		}

		l -= t->l;
		t += t->l;
	}

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




static u8
apic_csum (void *p, u8 bytes)
{
	u8 csum = 0;
	u8 *pp = (u8 *)p;

	while ((u64)pp < (u64)p + bytes)
		csum += *pp++;

	return csum;
}


static u64 ranges[][2] = {
	{ 0x9fc00 , 0xa0000 }, /* Extended BIOS Data Area (EBDA) */
	{ 0xe0000 , 0xfffff }, /* BIOS ROM */
	{ 0x00000 , 0x00000 }
};


static void *
search_table (u64 signature)
{
	u8 i = 0;
	u64 *p;

	do {
		if (ranges[i] == 0)
			break;

		for (p = (u64 *)ranges[i][0]; p < (u64 *)ranges[i][1];
			p += 2) {
			if (*p != signature)
				continue;

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


	rsdpp = (struct rsdp *)search_table (RSDP_SIG);
	if (rsdpp == NULL || rsdpp->rsdt == 0)
		kpanic ("No valid ACPI tables.");


	if (apic_csum (rsdpp, 20) != 0)
		kpanic ("Wrong checksum in rsdp\n");


	/* Is not null terminated.. */
	strncpy (rsdpp->oemid, oem, 6);
	oem[6] = '\0';

	kprintf ("  ACPI table v%d by %s\n", rsdpp->rev, oem);



	h = (struct acpi_header *)(u64)rsdpp->rsdt;
	if (h->sig != RSDT_SIG)
		kpanic ("Invalid RSDT in ACPI table\n");

	if (apic_csum (h, h->l) != 0)
		kpanic ("Wrong checksum in rsdt\n");


	for (p = (u32 *)&h->entry; p < (u32 *)((u64)h + h->l); p++) {

		struct acpi_header *h2 = (struct acpi_header *)(u64)*p;


		if (apic_csum (h2, h2->l) != 0)
			continue;

		/*kprintf ("%s\n", &h2->sig);*/

		switch (h2->sig) {
		case APIC_SIG:
			/* This is the mother of the lamb!!
			 * A bit confusing because the signature (APIC) 
			 * is different from the table name! (MADT) */
			acpi_walk_madt (
				(struct acpi_apic *)&h2->entry,
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
		}
	}

}




