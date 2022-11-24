#include "devId.h"

void devIdGet(char *DeviceID) {
    uint8_t *id = (uint8_t *)0x1FFFF7E8;
    for(uint8_t i = 0; i < 12; i++) {
        uint8_t tmp;

        tmp = (*id >> 4);
        *(DeviceID++) = tmp < 10? tmp + '0' : tmp - 10 + 'a';
        
        tmp = (*id & 0x0F);
        *(DeviceID++) = tmp < 10? tmp + '0' : tmp - 10 + 'a';

        id++;
    }
    *DeviceID = 0;
}