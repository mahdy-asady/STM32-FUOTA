#ifndef _STRING_H_
#define _STRING_H_

#include <stdarg.h>
#include <stdint.h>

void strconcat(char *Destination, int MaxLength, int Count, ...);

int strcmp(char* String1, char *String2);

char *Num2Str(uint32_t Number, char * Str);

#endif
