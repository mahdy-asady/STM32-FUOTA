#include "GPIO.h"
#include "USART.h"
#include "debug.h"
#include "timer.h"
#include "bootOptions.h"
#include "bkpreg.h"


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

void ButtonInterruptEnable(void) {
    //Enable Interrupt on PA0
    GPIO_EnablePort(GPIOA);
    GPIO_InitPin(GPIOA, PIN_00, PinOperationInputPullUpDown);
    //GPIO_WritePin(GPIOA, PIN_00, PinLow);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->EXTICR[1] = (AFIO->EXTICR[1] & ~AFIO_EXTICR2_EXTI4) | AFIO_EXTICR2_EXTI4_PA;
    EXTI->IMR |= EXTI_IMR_IM0;
    EXTI->RTSR |= EXTI_RTSR_RT0;
    EXTI->FTSR &= ~EXTI_FTSR_FT0;
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void InitSystem(void) {
    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_02, PinOperationOutputPushPull);

    UsartInit();
    TimerInit();
    BackupRegInit();

    ButtonInterruptEnable();
}

void EXTI0_IRQHandler(void) {
    if(EXTI->PR & EXTI_PR_PIF0) {
        log_info(&UsartDebug, "System set to update!!");
        BackupRegWrite(0, BOOT_UPDATE);
        NVIC_SystemReset();
        EXTI->PR |= EXTI_PR_PIF0;
    }
}
