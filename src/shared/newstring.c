#include "newstring.h"

void StrConcat(char *Destination, int MaxLength, int Count, ...){
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


int StrCompare(char* String1, char *String2) {
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

char *Num2Str(uint32_t Number, char * Str) {
    uint32_t Divider = 1000000000;
    uint8_t BiggestFound = 0;
    uint8_t Index = 0;

    while(Divider > 0) {
        uint8_t Result = Number / Divider;
        Number %= Divider;
        Divider /= 10;

        if(Result > 0 || BiggestFound) {
            BiggestFound = 1;
            Str[Index++] = '0' + Result;
        }
    }
    if(Index == 0)
        Str[Index++] = '0';
    
    Str[Index] = 0;
    return Str;
}
