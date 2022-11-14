#include "GPIO.h"
#include "USART.h"
#include "debug.h"
#include "timer.h"


USART_Handle UsartDebug;

void InitSystem(void);


int main(void)
{
    InitSystem();

    log_info(&UsartDebug, "Application Started!!");

    while (1)
    {
        GPIO_TogglePin(GPIOD, PIN_02);
        Delay_ms(500);
    }
}

void SystemInit(void) {}

void UsartInit(void) {
    UsartDebug.Instance = USART1;
    UsartDebug.BaudRate = 40 << 4;
    UsartDebug.isPortMapped = 0;
    USART_Init(&UsartDebug);
}

void InitSystem(void) {
    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_02, PinOperationOutputPushPull);
    UsartInit();
    TimerInit();
}
