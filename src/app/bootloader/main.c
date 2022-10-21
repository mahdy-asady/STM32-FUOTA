#include <stdint.h>
#include "stm32f107xc.h"
#include "GPIO.h"

extern char FLASH_APP1_OFFSET;

void BootApplication(void);
void ShowBootloaderSign(void);

int main(void) {

    ShowBootloaderSign();

    BootApplication();

    while(1);
}

void SystemInit(void) {

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
    GPIO_InitPin(GPIOD, PIN_04, PinDirectionOutput);

    for(int i = 0; i < 5; i++) {
        GPIO_TogglePin(GPIOD, PIN_04);
        for (volatile int j = 0; j < 100000; j++);
    }
    for (volatile int j = 0; j < 200000; j++);
    GPIO_TogglePin(GPIOD, PIN_04);

    //cleanup. Set anything as untouched
    GPIO_DisablePort(GPIOD);
}
