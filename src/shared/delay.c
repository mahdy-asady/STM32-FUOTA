#include <stdint.h>
#include "stm32f107xc.h"


void _delay_ms(uint16_t duration) {
    uint32_t Counter = duration * (F_CPU/8)/1000;
    SysTick->LOAD = Counter & 0xFFFFFF;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}