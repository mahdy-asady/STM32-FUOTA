#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

void EE_Init(void);

void EE_Write(uint32_t Address, uint32_t Value);

uint32_t EE_Read(uint32_t Address, uint32_t DefaultValue);


#endif