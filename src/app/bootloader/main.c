#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f107xc.h"
#include "GPIO.h"
#include "USART.h"
#include "ESP-AT.h"
#include "debug.h"
#include "timer.h"

extern char FLASH_APP1_OFFSET;

USART_Handle UsartDebug;
USART_Handle UsartWebConn;

void BootApplication(void);
void ShowBootloaderSign(void);
void SystemClockSetup(void);
void UsartInit(void);
bool GetUpdateInfo(uint32_t *FileVersion, char *FileName, uint32_t *FileSize, uint8_t *FileCRC);

int main(void) {

    ShowBootloaderSign();

    UsartInit();
    
    log_info(&UsartDebug, "Boot loader Started!");
    

    ESP_Init(&UsartWebConn, &UsartDebug);
    ESP_WifiConnect(WIFI_SSID, WIFI_PASS);

    uint32_t    UpdateVersion;
    char        FileName[255];
    uint32_t    FileSize;
    uint8_t     FileCRC[4];
    GetUpdateInfo(&UpdateVersion, FileName, &FileSize, FileCRC);

    log_info(&UsartDebug, "Done!!!");

    //BootApplication();

    while(1);
}


void SystemInit(void) {
    SystemClockSetup();
    TimerInit();
}

/*
    Set System Clock to 72Mhz
*/
void SystemClockSetup(void) {
    //Enable HSE & Wait to be ready
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    //SET PREDIV2 FACTOR TO 5
    RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PREDIV2) | RCC_CFGR2_PREDIV2_DIV5;

    //SET PLL2MUL FACTOR TO 8
    RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PLL2MUL) | RCC_CFGR2_PLL2MUL8;

    //ENABLE PLL2
    RCC->CR |= RCC_CR_PLL2ON;
    while (!(RCC->CR & RCC_CR_PLL2RDY));

    //SET PREDIV1 SRC TO PLLMULL2
    RCC->CFGR2 |= RCC_CFGR2_PREDIV1SRC_PLL2;

    //SET PREDIV1 FACTOR TO 5
    RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PREDIV1) | RCC_CFGR2_PREDIV1_DIV5;

    //SET PLL CLOCK SOURCE TO PREDIV1
    RCC->CFGR |= RCC_CFGR_PLLSRC;

    //Set PLLMul to 9
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLMULL) | RCC_CFGR_PLLMULL9;

    //Enable PLL & Wait to be ready
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    //Enable flash Prefetch buffer & Set Flash latency to 2
    FLASH->ACR |= FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

    //AHB NO Divider
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV1;

    //SET SW TO PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR & (RCC_CFGR_SWS_PLL)));

    //APB1 Divide by 2
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_CFGR_PPRE1_DIV2;

    //APB2 NO Divider
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
}


void UsartInit(void) {
    UsartDebug.Instance = USART1;
    UsartDebug.BaudRate = 40 << 4;
    UsartDebug.isPortMapped = 0;
    USART_Init(&UsartDebug);
    
    UsartWebConn.Instance = USART2;
    UsartWebConn.BaudRate = 20 << 4;
    UsartWebConn.isPortMapped = 1;
    USART_Init(&UsartWebConn);
}

void BootApplication(void) {
    void (*AppResetHandler)(void) = (void*)(*((volatile uint32_t*) (&FLASH_APP1_OFFSET + 4U)));

    //Set Vector Table
    SCB->VTOR = (uint32_t)&FLASH_APP1_OFFSET;
    // Set stack pointer
    __set_MSP((uint32_t)*((__IO uint32_t*)&FLASH_APP1_OFFSET));
    //Run Application's ResetHandler
    AppResetHandler();
}

void ShowBootloaderSign(void) {
    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_04, PinOperationOutputPushPull);

    for(int i = 0; i < 5; i++) {
        GPIO_TogglePin(GPIOD, PIN_04);
        Delay_ms(300);
    }
    Delay_ms(600);
    GPIO_TogglePin(GPIOD, PIN_04);

    //cleanup. Set anything as untouched
    GPIO_DisablePort(GPIOD);
}


/*
    update file data structure:
    Index   Byte Length     Content
    0       1               Version Patch
    1       1               Version Minor
    2       1               Version Major
    3       1               NULL
    4       1               File Name Length
    5       FileLength      File Name
    6       1               NULL
    7       4               File Size (In little Endian 32 bit number)
    8       4               File Content CRC32
    9       1               NULL

*/

bool GetUpdateInfo(uint32_t *FileVersion, char *FileName, uint32_t *FileSize, uint8_t *FileCRC) {
    uint8_t FileContentBuffer[200];
   
    uint8_t ContentSize = ESP_GetFileChunk(UPDATE_SERVER "/update", 0, 199, FileContentBuffer, 200);
    if(!ContentSize)
        return false;
    
    uint8_t AddressOffset = 0;

    *FileVersion = *((uint32_t*)FileContentBuffer);
    AddressOffset += 4;

    uint8_t FileNameLength = *((uint8_t*)(FileContentBuffer + AddressOffset));
    AddressOffset += 1;

    strlcpy(FileName, (char *)(FileContentBuffer + AddressOffset), FileNameLength + 1);
    AddressOffset += FileNameLength + 1;

    *FileSize = *((uint32_t*)(FileContentBuffer + AddressOffset));
    AddressOffset += 4;

    FileCRC[0] = *((uint8_t*)FileContentBuffer + AddressOffset++);
    FileCRC[1] = *((uint8_t*)FileContentBuffer + AddressOffset++);
    FileCRC[2] = *((uint8_t*)FileContentBuffer + AddressOffset++);
    FileCRC[3] = *((uint8_t*)FileContentBuffer + AddressOffset++);

    return true;
}
