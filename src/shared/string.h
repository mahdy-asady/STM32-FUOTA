#ifndef _STRING_H_
#define _STRING_H_

#include<stdarg.h>

void strconcat(char *Destination, int MaxLength, int Count, ...);

int strcmp(char* String1, char *String2);

#endif