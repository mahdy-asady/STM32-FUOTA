#ifndef _USART_H_
#define _USART_H_

#include "stm32f107xc.h"


//USART 1
void USART_EnableUSART1(void);
void USART_FetchUSART1(void (*CallBack)(char*));

//USART 2
void USART_EnableUSART2(void);
void USART_FetchUSART2(void (*CallBack)(char*));


void USART_SendString(USART_TypeDef*, char*);

#endif