#include <stdint.h>
#include "stm32f107xc.h"

void BackupRegInit(void) {
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;
}

uint32_t *GetSlot(uint8_t SlotIndex) {
    uint32_t *addr = (uint32_t *)BKP;
    addr++;
    if(SlotIndex > 10)
        addr += 5;
    return addr;
}

void BackupRegWrite(uint8_t SlotIndex, uint16_t Data) {
    *GetSlot(SlotIndex) = Data;
}

uint16_t BackupRegRead(uint8_t SlotIndex) {
    return *GetSlot(SlotIndex);
}
