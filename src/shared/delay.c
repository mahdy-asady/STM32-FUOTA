#include <stdint.h>
#include "stm32f107xc.h"

void DelaySet(uint16_t Duration) {
    uint32_t Counter = Duration * (F_CPU/8)/1000;
    SysTick->LOAD = Counter & 0xFFFFFF;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
}

int DelayCatched(void) {
    int Result = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
    if(Result)
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    return Result;
}

void _delay_ms(uint16_t Duration) {
    DelaySet(Duration);
    while(!DelayCatched());
}