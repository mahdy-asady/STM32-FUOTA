
#include<stdarg.h>

void strconcat(char *Destination, int MaxLength, int Count, ...){
    int CurrentLength = 0;
    
    va_list ptr;
    va_start(ptr, Count);

    for(int i = 0; i < Count; i++) {
        char *String = va_arg(ptr, char*);
        while(*String != 0 && CurrentLength < (MaxLength - 1)) {
            *Destination++ = *String++;
            CurrentLength++;
        }
    }
    va_end(ptr);

    *Destination = 0;
}


int strcmp(char* String1, char *String2) {
    while(1) {
        if(*String1 == 0 && *String2 == 0)
            return 0;
        
        if(*String1 < *String2)
            return -1;
        else if(*String1 > *String2)
            return 1;
        String1++;
        String2++;
    }
}