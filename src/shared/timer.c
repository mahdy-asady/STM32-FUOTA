#include <stdint.h>
#include "stm32f107xc.h"

void TimerInit(void) {
    //Setup Timer2 as 1ms Timer Counter
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 36000 - 1; //0.5ms : 2kHz
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint16_t GetSysTick(void) {
    return TIM2->CNT;
}

int TimeoutReached(uint16_t _SysTick, uint16_t Timeout) {
    uint16_t TimeoutTick = Timeout * 2;

    if((uint16_t)(TIM2->CNT - _SysTick) < TimeoutTick)
        return 0;
    return 1;
}


void Delay_ms(uint16_t MiliSeconds) {
    uint16_t StartTime = TIM2->CNT;
    uint16_t Ticks = MiliSeconds *= 2;

    while((uint16_t)(TIM2->CNT - StartTime) < Ticks);
}
