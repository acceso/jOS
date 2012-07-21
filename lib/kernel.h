
#ifndef LIB_KERNEL_H
#define LIB_KERNEL_H


#define K_CS 0x8
#define K_DS 0x10
#define K_SS 0x18

#define STACKSIZE (8 * 1024)




#ifndef _ASM /* Not for assembly */



void kpanic (char *str);



#endif /* _ASM */

#endif /* LIB_KERNEL_H */

