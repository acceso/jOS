
#ifndef STRING_H
#define STRING_H


#include <stdint.h>



s8 strcmp (const char *s1, const char *s2);
s8 strncmp (const char *s1, const char *s2, size_t l);
void strcpy (char *dst, char *src);
void strncpy (char *dst, char *src, s64 l);
void memcpy (void *dst, void *src, size_t l);
size_t strlen (const char *s);
char *strchr (const char *s, int c);
size_t strcspn (const char *s1, const char *s2);
size_t strspn (const char *s1, const char *s2);
void *memset (void *s, u8 c, size_t n);
s16 strlen2sep (const char *s, const char sep);


#endif /* STRING_H */


