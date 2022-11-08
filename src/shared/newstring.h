#ifndef _NEWSTRING_H_
#define _NEWSTRING_H_

#include <stdarg.h>
#include <stdint.h>

void strconcat(char *Destination, int MaxLength, int Count, ...);

int StrCompare(char* String1, char *String2);

char *Num2Str(uint32_t Number, char * Str);

#endif
