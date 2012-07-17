
#ifndef STRING_H
#define STRING_H


#include <stdint.h>



s8 strcmp (const char *s1, const char *s2);
s8 strncmp (const char *s1, const char *s2, u16 l);
void strcpy (char *dst, char *src);
void strncpy (char *dst, char *src, s16 l);
u16 strlen (char *s);



#endif /* STRING_H */


