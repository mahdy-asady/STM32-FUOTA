#ifndef _BKPREG_H_
#define _BKPREG_H_

#include <stdint.h>

void BackupRegInit(void);

void BackupRegWrite(uint8_t SlotIndex, uint16_t Data);

uint16_t BackupRegRead(uint8_t SlotIndex);

#endif