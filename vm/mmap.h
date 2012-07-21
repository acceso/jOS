
#ifndef VM_MMAP_H
#define VM_MMAP_H


#define PROT_READ	0x1
#define PROT_WRITE	0x2
#define PROT_EXEC	0x4

#define MAP_SHARED	0x01
#define MAP_PRIVATE	0x02
#define MAP_FIXED	0x10
#define MAP_ANONYMOUS	0x20



void *mmap (void *addr, size_t length, int prot, int flags, int fd, off_t offset);




#endif /* VM_MMAP_H */

