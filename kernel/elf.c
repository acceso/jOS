

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fs/file.h>
#include <fs/filedesc.h>
#include <vm/mmap.h>




/* Main ELF header: */
struct elf_header {
	u8 e_ident[16];
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	u64 e_entry;	/* Entry point (virt addr) or zero */
	u64 e_phoff;	/* Program header table offset or zero */
	u64 e_shoff;	/* Section header table offset or zero */
	u32 e_flags;	/* CPU flags */
	u16 e_ehsize;
	u16 e_phentsize;/* Size of one entry in the program header table */
	u16 e_phnum;	/* Number of entries in the program header table */
	u16 e_shentsize;/* Size of one entry in the section header table */
	u16 e_shnum;	/* Number of entries in the section header table */
	u16 e_shstrndx; /* Idx in the section header table 
			 * associated with the section name string table */
} __attribute__ ((packed));


/* elf_header->e_ident fields */
#define EI_CLASS	4
#define EI_DATA		5
#define EI_VERSION	6
#define EI_OSABI	7
#define EI_ABIVERSION	8
#define EI_PAD		9
#define EI_NIDENT	16

#define ELFCLASS32	1
#define ELFCLASS64	2

#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define ELFOSABI_SYSV	0


/* elf_hader->e_type */
#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC		2
#define ET_DYN		3
#define ET_CORE		4

/* elf_hader->e_machine */
#define EM_X86_64	0x3e



struct elf_ph_entry {
	u32 ph_type;	/* Type of segment */
	u32 ph_flags;	/* Segment attributes */
	u64 ph_offset;	/* Offset in file */
	u64 ph_vaddr;	/* Virtual address in memory */
	u64 ph_paddr;	/* Reserved */
	u64 ph_filesz;	/* Size of segment in file */
	u64 ph_memsz;	/* Size of segment in memory */
	u64 ph_align;	/* Alignment of segment */
} __attribute__ ((packed));


/* elf_ph_entry->ph_type */
#define PT_NULL		0 		/* Unused entry */
#define PT_LOAD		1 		/* Loadable segment */
#define PT_DYNAMIC	2 		/* Dynamic linking tables */
#define PT_INTERP	3 		/* Program interpreter path name */
#define PT_NOTE		4 		/* Note sections */
#define PT_SHLIB	5 		/* Reserved */
#define PT_PHDR		6 		/* Program header table */
#define PT_LOOS		0x60000000	/* Environment-specific use */
#define PT_HIOS		0x6fffffff
#define PT_LOPROC	0x70000000	/* Processor-specific use */
#define PT_HIPROC	0x7fffffff




static u16
elf_get_machine (void)
{
	return EM_X86_64;
}



static s8
elf_check_header (struct elf_header *elfh, u16 e_type)
{
	if (strncmp ((char *)elfh, "\x7f" "ELF", 4) != 0)
		return -1;

	if (elfh->e_ident[EI_CLASS] != ELFCLASS64)
		return -1;

	if (elfh->e_ident[EI_DATA] != ELFDATA2LSB)
		return -1;

	if (elfh->e_ident[EI_VERSION] != 1)
		return -1;

	if (elfh->e_ident[EI_OSABI] != ELFOSABI_SYSV
		|| elfh->e_ident[EI_ABIVERSION] != 0)
		return -1;

	if (elfh->e_type != e_type)
		return -1;

	if (elfh->e_machine != elf_get_machine ())
		return -1;

	return 0;
}



static s8
elf_load_section (s32 fd, struct elf_ph_entry *ph)
{
	if (ph->ph_vaddr % ph->ph_align > 0)
		return -1;

	mmap ((void *)ph->ph_vaddr, ph->ph_memsz, PROT_EXEC, 
		MAP_SHARED | MAP_FIXED, fd, ph->ph_offset);

	return 0;
}



/* As elf will be mapped on lower half, a signed is big enough. */
s64
elf_map (const char *path)
{
	s32 fd;
	struct elf_header elfh;
	struct elf_ph_entry elf_ph;
	u32 bytes;
	u64 offset;
	u16 cur;

	fd = open (path, 0, 0);
	if (fd == -1)
		return -1;

	bytes = read (fd, &elfh, sizeof (struct elf_header));
	if (bytes < sizeof (struct elf_header))
		return -1;

	if (elf_check_header (&elfh, ET_EXEC) == -1)
		return -1;

	if (elfh.e_phoff == 0 || elfh.e_phnum == 0)
		return -1;

	offset = elfh.e_phoff;
	cur = elfh.e_phnum;

	do {
		lseek (fd, offset, SEEK_SET);

		bytes = read (fd, &elf_ph, sizeof (struct elf_ph_entry));
		if (bytes < sizeof (struct elf_ph_entry))
			return -1;


		switch (elf_ph.ph_type) {
		case PT_NULL:
			break;
		case PT_LOAD:
			{
			s8 ret;

			ret = elf_load_section (fd, &elf_ph);
			if (ret == -1)
				return -1;
			}
			break;
		case PT_DYNAMIC:
			break;
		case PT_INTERP:
			break;
		case PT_NOTE:
			break;
		case PT_SHLIB:
			break;
		case PT_PHDR:
			break;
		case PT_LOOS:
			break;
		case PT_HIOS:
			break;
		case PT_LOPROC:
			break;
		case PT_HIPROC:
			break;
		}


		offset += elfh.e_phentsize;
	} while (--cur);


	return elfh.e_entry;
}





















/* These will be useful in the future: */



/* Section indexes */
#define SHN_UNDEF	0	/* Meaningless section reference */
#define SHN_LOPROC	0xff00	/* Processor-specific use */
#define SHN_HIPORC	0xff1f
#define SHN_LOOS	0xff20	/* Environment-specific use */
#define SHN_HIOS	0xff3f
#define SHN_ABS		0xfff1	/* Absolute value */
#define SHN_COMMON	0xfff2	/* Common block (unallocated c external vars)
				 */


/* Each section is described with an entry like this one: */
struct elf_sect_header {
	u32 sh_name;	/* Name, as an index into the section table header
			 * string table */
	u32 sh_type;	/* Type of section, see SHT_* macros */
	u64 sh_flags;	/* See SHF_* macros */
	u64 sh_addr;	/* VA where the section will be loaded or 0 */
	u64 sh_offset;	/* File offset where the section begins */
	u64 sh_size;	/* Section size, 0 for SHT_NOBITS */
	u32 sh_link;	/* Associated section (depends on the type) */
	u32 sh_info;	/* Extra info (also depends on the type) */
	u64 sh_addralign; /* Required alignment */
	u64 sh_entsize;	/* Size of each entry for sections with fixed-size 
			 * entries */
} __attribute__ ((packed));


#define SHT_NULL	0 /* Inactive. No section associated. */
#define SHT_PROGBITS	1 /* Information defined by the program */
#define SHT_SYMTAB	2 /* Symbol table */
#define SHT_STRTAB	3 /* String table */
#define SHT_RELA	4 /* Relocation entries with explicit addens */
#define SHT_HASH	5 /* Symbol hash table */
#define SHT_DYNAMIC	6 /* For dynamic linking */
#define SHT_NOTE	7 /* Misc info */
#define SHT_NOBITS	8 /* Like PROGBITS but occupies no space */
#define SHT_REL		9 /* Relocations without explicit addens */
#define SHT_SHLIB	10 /* Reserved */
#define SHT_DYNSYM	11 /* Symbol table */
#define SHT_LOOS	0x60000000 /* Range reserved for environment use */
#define SHT_HIOS	0x6ff77777 /* */
#define SHT_LOPROC	0x70000000 /* Values in this range are reserved */
#define SHT_HIPROC	0x7fffffff /* for processor-specific semantics. */

#define SHF_WRITE	0x1 /* Section has data that should be writable 
			     * during execution */
#define SHF_ALLOC	0x2 /* Section occupies memory */
#define SHF_EXECINSTR	0x4 /* Section contains instructions */
#define SHF_MASKOS	0x0f000000 /* Reserved for environment */
#define SHF_MASKPROC	0xf0000000 /* Reserved for processor */




/* Segment attributes */
#define PF_X		0x1		/* Execute permission */
#define PF_W		0x2		/* Write permission */
#define PF_R		0x4		/* Read permission */
#define PF_MASKOS	0x00ff0000	/* Environment-specific use */
#define PF_MASKPROC	0xff000000	/* Processor-specific use */



struct elf_sym {
	u32 st_name;	/* Symbol name. */
	u8 stinfo;	/* Symbol type and binding attributes (4+4 bits). */
	u8 st_other;	/* Reserved. Must be zero. */
	u16 st_shndx;	/* Section table index or SHN_UNDEF */
	u64 st_value;	/* Value of the symbol. Absolute or relocatable. */
	u64 st_size;	/* Size of object or zero. */
} __attribute__ ((packed));


#define STB_LOCAL	0	/* Not visible outside object file */
#define STB_GLOBAL	1	/* Global, visible to all object files */
#define STB_WEAK	2	/* Global but lower precedence. */
#define STB_LOOS	10	/* Environment-specific use */
#define STB_HIOS	12
#define STB_LOPROC	13	/* Processor-specific use */
#define STB_HIPROC	15

#define STT_NOTYPE	0	/* No type, eg. absolute symbol */
#define STT_OBJECT	1	/* Data object */
#define STT_FUNC	2	/* Function entry point */
#define STT_SECTION	3	/* Symbol associated with a section */
#define STT_FILE	4	/* Source file associated with object file */
#define STT_LOOS	10	/* Environment-specific use */
#define STT_HIOS	12
#define STT_LOPROC	13	/* Processor-specific use */
#define STT_HIPROC	15






