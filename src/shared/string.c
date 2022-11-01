

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