

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fs/file.h>
#include <fs/filedesc.h>




struct elf_header {
	u8 e_ident[16];
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	u64 e_entry;
	u64 e_phoff;
	u64 e_shoff;
	u32 e_flags;
	u16 e_ehsize;
	u16 e_phentsize;
	u16 e_phnum;
	u16 e_shentsize;
	u16 e_shnum;
	u16 e_shstrndx;
} __attribute__ ((packed));



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

#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC		2
#define ET_DYN		3
#define ET_CORE		4



struct elf_sect_header {
	u32 sh_name;
	u32 sh_type;
	u64 sh_flags;
	u64 sh_addr;
	u64 sh_off;
	u64 sh_size;
	u32 sh_link;
	u32 sh_info;
	u64 sh_addralign;
	u64 sh_entsize;
} __attribute__ ((packed));



#define EM_X86_64	0x3e


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



s32
elf_load (char *path)
{
	s32 fd;
	struct elf_header elfh;
	u32 bytes;

	fd = open (path, 0, 0);
	if (fd == -1)
		return -1;

	bytes = read (fd, &elfh, sizeof (struct elf_header));
	if (bytes < sizeof (struct elf_header))
		return -1;

	if (elf_check_header (&elfh, ET_EXEC) == -1)
		return -1;

	lseek (fd, elfh.e_phoff, SEEK_SET);

//	TODO: .....


	return 1;
}




