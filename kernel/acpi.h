
#ifndef ACPI_H
#define ACPI_H


#include <stdint.h>


/* The problem with this is that is not a number
 * and it uses storage, could be useful though: */
#define ACPI_MAGIC(_x)	(*(u32 *)_x)


static inline u8
acpi_csum (void *p, u8 bytes)
{
	u8 csum = 0;
	u8 *pp = (u8 *)p;

	while ((u64)pp < (u64)p + bytes)
		csum += *pp++;

	return csum;
}


void *acpi_search_table (u64 signature);

void init_acpi (void);


#endif /* ACPI_H */

