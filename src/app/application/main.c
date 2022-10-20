#include "GPIO.h"

int main(void)
{
    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_02, PinDirectionOutput);

    while (1)
    {
        GPIO_TogglePin(GPIOD, PIN_02);
        for (volatile int i = 0; i < 500000; i++);
    }
}

void SystemInit(void) {

}
