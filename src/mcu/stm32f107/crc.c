#include <stdint.h>
#include "stm32f107xc.h"

uint32_t CRC_Calculate(uint32_t *Data, uint32_t Length) {
    uint32_t CRCResult;
    
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    CRC->CR |= CRC_CR_RESET;
    for(uint32_t i = 0; i < Length; i++) {
        CRC->DR = *(Data + i);
    }
    CRCResult = CRC->DR;
    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
    return CRCResult;
}