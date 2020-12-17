
#ifndef LIB_KERNEL_H
#define LIB_KERNEL_H


#define K_CS 0x20
#define K_DS 0x28
#define U_CS 0x30
#define U_DS 0x38


#define STACKSIZE (8 * 1024)



#ifndef _ASM /* Not for assembly */



void kpanic(char *str);



#endif /* _ASM */

#endif /* LIB_KERNEL_H */

