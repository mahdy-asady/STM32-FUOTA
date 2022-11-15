#include <stdbool.h>
#include <stdint.h>
#include "eeprom.h"
#include "flash.h"

#define FLASH_UINT32_COUNT    0x200U
#define SLOT_IS_FREE          0xFFFFFFFF

extern char FLASH_DATA_OFFSET;

uint32_t *ActivePage;
uint32_t *NextFreeSlot;

uint32_t * FindAddressLastSlot(uint32_t Address) {
    uint32_t *SearchingSlot  = NextFreeSlot - 2;
    while (SearchingSlot > ActivePage)
    {
        if(*SearchingSlot == Address) {
            return SearchingSlot;
        }

        SearchingSlot -= 2;
    }
    return 0;
}

void PrepareNextPage(void) {
    uint32_t *OldPage = ActivePage;
    if(ActivePage == (uint32_t *)&FLASH_DATA_OFFSET)
        ActivePage += FLASH_UINT32_COUNT;
    else
        ActivePage -= FLASH_UINT32_COUNT;
    NextFreeSlot = ActivePage;
    //1. write previous variables to this one
    uint32_t *SearchingSlot = OldPage + FLASH_UINT32_COUNT - 2;
    while(SearchingSlot > OldPage) {
        if(!FindAddressLastSlot(*SearchingSlot))
            EE_Write(*SearchingSlot, *(SearchingSlot + 1));
        SearchingSlot -= 2;
    }
    //2. erase previous page
    FlashErase((uint32_t)OldPage, 0);
}

void EE_Init(void) {
    FlashUnlock();
    //find active page for memory access
    ActivePage = (uint32_t *)&FLASH_DATA_OFFSET;
    if((*(uint32_t *)&FLASH_DATA_OFFSET) == SLOT_IS_FREE)
        ActivePage += FLASH_UINT32_COUNT;

    //find first unused location
    NextFreeSlot = ActivePage;
    while (*NextFreeSlot != SLOT_IS_FREE)
    {
        NextFreeSlot += 2;
    }
}

void EE_Write(uint32_t Address, uint32_t Value) {
    if((ActivePage + FLASH_UINT32_COUNT) <= NextFreeSlot)
        PrepareNextPage();
    FlashWrite((uint32_t)NextFreeSlot, (uint8_t *)&Address, 4);
    FlashWrite((uint32_t)(NextFreeSlot + 1), (uint8_t *)&Value, 4);
    //*NextFreeSlot++ = Address;
    //*NextFreeSlot++ = Value;
}

uint32_t EE_Read(uint32_t Address, uint32_t DefaultValue) {
    uint32_t *SearchingSlot = FindAddressLastSlot(Address);
    if(SearchingSlot) {
        return *(SearchingSlot + 1);
    }
        
    return DefaultValue;
}