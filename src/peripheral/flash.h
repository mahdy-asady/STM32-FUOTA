#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>
#include <stdbool.h>

// Tries to unlock flash memory, return true on success, false on failure
bool FlashUnlock(void);

// Locks flash memory for preventing accident write
void FlashLock(void);

//Erase flash memory starting at StartAddree to the length. 
//note that it erases pages, so it will erase data before startAddress and after Length if the addresses not chosen properly
bool FlashErase(uint32_t StartAddress, uint32_t Length);

// Write Data of length Length to given flash address. note that Length should be factor of 2
bool FlashWrite(uint32_t StartAddress, uint8_t *Data, uint32_t Length);

#endif