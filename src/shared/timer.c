#include <stdint.h>
#include "stm32f107xc.h"

void _delaySet(uint32_t Loader) {
    SysTick->LOAD = Loader & 0xFFFFFF;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
}

int _delayCatched(void) {
    int Result = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
    return Result;
}

uint32_t SetTimeout(uint32_t MiliSeconds) {
    uint32_t TotalTicks = MiliSeconds * ((F_CPU/8UL)/1000);
    uint32_t Counter = (TotalTicks >> 24) + 1;
    uint32_t LoadValue = TotalTicks / Counter;
    _delaySet(LoadValue);
    return Counter;
}

int TimeoutReached(uint32_t *Counter) {
    if(_delayCatched())
        (*Counter)--;
    if((*Counter) == 0) {
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        return 1;
    }
    return 0;
}


void Delay_ms(uint16_t MiliSeconds) {
    uint32_t Counter = SetTimeout(MiliSeconds);
    
    while(!TimeoutReached(&Counter));
}