#include <stdint.h>
#include "stm32f107xc.h"
#include "GPIO.h"
#include "USART.h"

extern char FLASH_APP1_OFFSET;

void BootApplication(void);
void ShowBootloaderSign(void);
void GetEsp32Data(char *Data);

int main(void) {

    ShowBootloaderSign();

    USART_EnableUSART1();

    USART_EnableUSART2();
    USART_FetchUSART2(&GetEsp32Data);
    
    char *Text = "Boot loader Started!!\r\n";
    USART_SendString(USART1, Text);

    char *Text2 = "AT+RST\r\n";
    USART_SendString(USART2, Text2);


    //BootApplication();

    while(1);
}

void GetEsp32Data(char *Data) {
    USART_SendString(USART1, Data);
}

/*
    Set System Clock to 72Mhz
*/
void SystemInit(void) {
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
        for (volatile int j = 0; j < 300000; j++);
    }
    for (volatile int j = 0; j < 600000; j++);
    GPIO_TogglePin(GPIOD, PIN_04);

    //cleanup. Set anything as untouched
    GPIO_DisablePort(GPIOD);
}
