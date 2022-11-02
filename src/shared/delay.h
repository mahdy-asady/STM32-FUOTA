#ifndef _DELAY_H_
#define _DELAY_H_

void Delay_ms(uint16_t MiliSeconds);

uint32_t SetTimeout(uint32_t MiliSeconds);
int TimeoutReached(uint32_t *Counter);

#endif