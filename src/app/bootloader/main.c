#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f107xc.h"
#include "board.h"
#include "USART.h"
#include "ESP-AT.h"
#include "debug.h"
#include "timer.h"
#include "fuota.h"
#include "bkpreg.h"
#include "bootOptions.h"
#include "newstring.h"
#include "eeprom.h"

extern char FLASH_APP1_OFFSET;

USART_Handle UsartDebug;
USART_Handle UsartWebConn;

void BootApplication(void);
void UsartInit(void);


int main(void) {

    ShowBootloaderSign();

    UsartInit();

    BackupRegInit();
    
    EE_Init();

    log_info(&UsartDebug, "Boot loader Started!");
    
    uint16_t BootCommand = BackupRegRead(0);
    switch (BootCommand)
    {
    case BOOT_UPDATE:
        log_info(&UsartDebug, "Do: Update");
        ESP_Init(&UsartWebConn, &UsartDebug);
        ESP_WifiConnect(WIFI_SSID, WIFI_PASS);
        FUOTA_Update();
        BackupRegWrite(0, BOOT_NORMAL);
        NVIC_SystemReset();
        break;
    
    case BOOT_BACKUP:
        log_info(&UsartDebug, "Do: Backup");

        FUOTA_Backup();
        
        BackupRegWrite(0, BOOT_NORMAL);
        NVIC_SystemReset();
        break;
    
    case BOOT_RESTORE:
        log_info(&UsartDebug, "Do: Restore");
        
        FUOTA_Restore();

        BackupRegWrite(0, BOOT_NORMAL);
        NVIC_SystemReset();
        break;
    
    case BOOT_NORMAL:
    default:
        log_info(&UsartDebug, "Do: Normal Boot");
        BootApplication();
        break;
    }

    while(1);
}


void SystemInit(void) {
    SystemClockSetup();
    TimerInit();
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
