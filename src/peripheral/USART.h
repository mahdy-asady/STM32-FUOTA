#ifndef _USART_H_
#define _USART_H_

#include "stm32f107xc.h"
#include "fifo.h"

typedef struct __USART_Handle {
    USART_TypeDef  *Instance;
    int             isPortMapped;
    uint32_t        BaudRate;
    FIFO_List       Buffer;
} USART_Handle;


void USART_Init(USART_Handle *);


void USART_SendString(USART_Handle*, char*);

void USART_WriteLine(USART_Handle*, char*);

int USART_BufferPopWithTimeout(USART_Handle *USART, uint8_t *ReturnData, uint32_t Timeout);

int USART_ReadLine(USART_Handle*, char*, uint8_t);

#endif