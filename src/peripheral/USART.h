#ifndef _USART_H_
#define _USART_H_

#include "stm32f107xc.h"

void USART_EnableUSART1(void);

void USART_FetchUSART1(void (*CallBack)(char));


void USART_SendString(USART_TypeDef*, char*);

#endif