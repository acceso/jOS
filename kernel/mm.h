
#ifndef MM_H
#define MM_H



#include <inc/types.h>

/*
 * The multiboot.h version that comes along Grub is full of "unsigned long" definitions.
 * They are 4 bits long in x32 but 8 in x64 !!
 */

typedef struct multiboot_info64 {
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	u32 boot_device;
	u32 cmdline;
	u32 mods_count;
	u32 mods_addr;
	u64 a, b;
	u32 mmap_length;
	u32 mmap_addr;
} multiboot_info64_t;


typedef struct multiboot_memory_map64 {
	u32 size;
	//unsigned long base_addr_low;
	//unsigned long base_addr_high;
	u64 base_addr;
	//unsigned long length_low;
	//unsigned long length_high;
	u64 length;
	u32 type;
} multiboot_memory_map64_t;





void init_memory (void);





#endif /* MM_H */

