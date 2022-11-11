#include "flash.h"
#include "stm32f107xc.h"
#include "USART.h"
#include "newstring.h"
#include "debug.h"


#define FLASH_PAGE_SIZE          0x800U

extern USART_Handle UsartDebug;

bool FlashUnlock(void) {
    if(FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
        if(FLASH->CR & FLASH_CR_LOCK)
            return false;
    }
    return true;
}

void FlashLock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

bool FlashErase(uint32_t StartAddress, uint32_t Length) {
    if((StartAddress < FLASH_BASE) || ((StartAddress + Length) > FLASH_BANK1_END))
        return false;
        
    uint32_t FirstPage = (StartAddress - FLASH_BASE) / FLASH_PAGE_SIZE;
    uint32_t LastPage = (StartAddress + Length - FLASH_BASE) / FLASH_PAGE_SIZE;
    //if((StartAddress + Length - FLASH_BASE) % FLASH_PAGE_SIZE)
    //    LastPage++;
    
    for(uint8_t i = FirstPage; i <= LastPage; i++) {
        while(FLASH->SR & FLASH_SR_BSY);
        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = FLASH_BASE + FLASH_PAGE_SIZE * i;
        FLASH->CR |= FLASH_CR_STRT;
    }
    while(FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PER;

    return true;
}

void WriteHalfWord(uint32_t Address, uint16_t Data) {
    FLASH->CR |= FLASH_CR_PG;
    *(volatile uint16_t *)Address = Data;
}

bool FlashWrite(uint32_t StartAddress, uint8_t *Data, uint32_t Length) {
    for(uint32_t i = 0; i < Length; i+=2) {
        WriteHalfWord(StartAddress + i, *(uint16_t *)(Data + i));
        while(FLASH->SR & FLASH_SR_BSY);
    }
    //we are writeing data in half-word length. so if data Count is odd we have to write a single byte + 0xFFFF
    if(Length % 2) {
        uint32_t LastLocation = Length - 1;
        uint16_t OddValue = *(Data + LastLocation) << 8;
        WriteHalfWord(StartAddress + LastLocation, OddValue);
    }

    return true;
}
