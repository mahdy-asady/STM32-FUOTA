#ifndef _USART_H_
#define _USART_H_

#include "stm32f107xc.h"

#define USART_BUFFER_SIZE 200

typedef struct __USART_Buffer {
    char            Content[USART_BUFFER_SIZE];
    char           *Start;
    char           *End;
} USART_Buffer;

typedef struct __USART_Handle {
    USART_TypeDef  *Instance;
    int             isPortMapped;
    uint32_t        BaudRate;
    USART_Buffer    Buffer;

} USART_Handle;


void USART_Init(USART_Handle *);


void USART_SendString(USART_Handle*, char*);

void USART_WriteLine(USART_Handle*, char*);

int USART_GetByte(USART_Handle *USART, uint8_t *ReturnData);

int USART_ReadLine(USART_Handle*, char*, uint8_t);

#endif