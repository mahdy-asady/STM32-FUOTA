#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef struct __FIFO_List {
    uint8_t            Content[FIFO_BUFFER_SIZE];
    uint8_t           *Start;
    uint8_t           *End;
} FIFO_List;

void FIFO_Init(FIFO_List *FIFO);
void FIFO_Push(FIFO_List *FIFO, uint8_t Data);
uint8_t FIFO_Pop(FIFO_List *FIFO);

bool FIFO_IsEmpty(FIFO_List *FIFO);
bool FIFO_IsFull(FIFO_List *FIFO);



#endif
