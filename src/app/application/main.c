#include "GPIO.h"
#include "USART.h"

int main(void)
{
    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_02, PinOperationOutputPushPull);

    USART_EnableUSART1();
    
    char *Text = "Application Started!!\r\n";
    USART_SendString(USART1, Text);

    while (1)
    {
        GPIO_TogglePin(GPIOD, PIN_02);
        for (volatile int i = 0; i < 500000; i++);
    }
}

void SystemInit(void) {

}
