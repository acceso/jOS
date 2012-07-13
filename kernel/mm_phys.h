
#ifndef MM_PHYS
#define MM_PHYS




#define PG_COUNT	0XFF
#define PG_LOCKED	(0x1 << 8)



void build_page_frames (void);

u64 *get_pages (u32 order);
u64 *get_page (void);


#endif /* MM_PHYS */


