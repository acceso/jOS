#ifndef PHYS
#define PHYS



#define PG_COUNT	0XFF
#define PG_LOCKED	(0x1 << 8)




void build_page_frames (void);

void *get_pages (u32 order);
void *get_one_page (void);

void free_pages (u64 *addr);
void free_pages_pfn (u64 pfn);


#endif /* PHYS */


