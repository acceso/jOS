
#ifndef TEST_TEST_H
#define TEST_TEST_H



/* I can't find an easy way to include system headers
 * so I place here the definitions I need. */

extern int puts (__const char *__s);
extern int printf (__const char *__restrict __format, ...);

extern void *malloc (size_t __size);
extern void free (void *__ptr);

void exit (int status);



#endif /* TEST_TEST_H */

