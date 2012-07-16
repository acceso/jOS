
#ifndef CTYPE_H
#define CTYPE_H


#include <stdint.h>



static inline u8
isdigit (char c)
{
	return c >= '0' && c <= '9';
}



static inline u8
islower (char c)
{
	return c >= 'a' && c <= 'z';
}



static inline u8
isupper (char c)
{
	return c >= 'A' && c <= 'Z';
}



static inline u8
tolower (char c)
{
	return isupper (c) ? c - 'A' + 'a' : c;
}



static inline u8
toupper (char c)
{
	return islower (c) ? c - 'a' + 'A' : c;
}





#endif /* CTYPE_H */


