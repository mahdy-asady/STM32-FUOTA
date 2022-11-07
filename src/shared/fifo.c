#include "fifo.h"

void FIFO_Init(FIFO_List *FIFO) {
    FIFO->Start = FIFO->End = FIFO->Content;
}

uint8_t *_NextIndex(FIFO_List *FIFO, uint8_t *Index) {
    if((Index + 1) == (FIFO->Content + FIFO_BUFFER_SIZE))
        return FIFO->Content;
    else
        return Index + 1;
}

void FIFO_Push(FIFO_List *FIFO, uint8_t Data) {
    *(FIFO->End) = Data;
    FIFO->End = _NextIndex(FIFO, FIFO->End);
}

uint8_t FIFO_Pop(FIFO_List *FIFO) {
    uint8_t Data = *(FIFO->Start);
    FIFO->Start = _NextIndex(FIFO, FIFO->Start);
    return Data;
}


bool FIFO_IsEmpty(FIFO_List *FIFO) {
    return FIFO->Start == FIFO->End;
}

bool FIFO_IsFull(FIFO_List *FIFO) {
    return FIFO->Start == _NextIndex(FIFO, FIFO->End);
}