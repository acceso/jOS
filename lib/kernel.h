
#ifndef KERNEL_H
#define KERNEL_H


#define K_CS 0x8
#define K_DS 0x10





#ifndef _ASM /* Not for assembly */



void kpanic (char *str);



#endif /* _ASM */

#endif /* KERNEL_H */

