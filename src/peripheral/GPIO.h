#ifndef _GPIO_H_
#define _GPIO_H_

#include "stm32f107xc.h"

typedef enum {
    PIN_00, PIN_01, PIN_02, PIN_03, PIN_04, PIN_05, PIN_06, PIN_07,
    PIN_08, PIN_09, PIN_10, PIN_11, PIN_12, PIN_13, PIN_14, PIN_15
} PINS;

typedef enum {PinLow, PinHigh} PinValues;

typedef enum {
    PinOperationAnalog,

    PinOperationInputFloating,
    PinOperationInputPullUpDown,

    PinOperationOutputPushPull,
    PinOperationOutputOpen,
    PinOperationOutputAlternatePushPull,
    PinOperationOutputAlternateOpen
} PinOperations;




void GPIO_EnablePort(GPIO_TypeDef *Port);
void GPIO_DisablePort(GPIO_TypeDef *Port);

void GPIO_InitPin(GPIO_TypeDef *Port, PINS PinNumber, PinOperations PinOperation);

void GPIO_WritePin(GPIO_TypeDef *Port, PINS PinNumber, PinValues PinValue);

int GPIO_ReadPin(GPIO_TypeDef *Port, PINS PinNumber);

void GPIO_TogglePin(GPIO_TypeDef *Port, PINS PinNumber);

#endif