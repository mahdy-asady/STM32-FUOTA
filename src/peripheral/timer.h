#ifndef _DELAY_H_
#define _DELAY_H_

void TimerInit(void);

void Delay_ms(uint16_t MiliSeconds);

uint16_t GetSysTick(void);
int TimeoutReached(uint16_t _SysTick, uint16_t Timeout);

#endif