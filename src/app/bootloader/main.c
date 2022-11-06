#include <stdint.h>
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

int main(void) {

    ShowBootloaderSign();

    UsartInit();
    
    log_info(&UsartDebug, "Boot loader Started!");
    

    ESP_Init(&UsartWebConn, &UsartDebug);
    ESP_WifiConnect(WIFI_SSID, WIFI_PASS);

    char Buffer[100];
    ESP_GetURL(UPDATE_SERVER "/update",Buffer,100);

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
