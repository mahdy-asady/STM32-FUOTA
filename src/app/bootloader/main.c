#include <stdint.h>
#include "stm32f107xc.h"
#include "GPIO.h"

extern char FLASH_APP1_OFFSET;

int main(void) {
    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_04, PinDirectionOutput);
    GPIO_TogglePin(GPIOD, PIN_04);
    for (volatile int i = 0; i < 500000; i++);
    GPIO_TogglePin(GPIOD, PIN_04);

    //cleanup. Set anything as untouched
    GPIO_DisablePort(GPIOD);

    //Set Vector Table
    SCB->VTOR = (uint32_t)&FLASH_APP1_OFFSET;

    //Run Application's ResetHandler
    void (*app_reset_handler)(void) = (void*)(*((volatile uint32_t*) (&FLASH_APP1_OFFSET + 4U)));
    app_reset_handler();

    while(1);
}

void SystemInit(void) {

}