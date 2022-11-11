#include <stddef.h>
#include "GPIO.h"
#include "stm32f107xc.h"

uint32_t RCC_SelectMask(GPIO_TypeDef *Port) {
    uint32_t Mask = 0;
    uint32_t PortAddress = (uint32_t)Port;
    switch(PortAddress) {
        case GPIOA_BASE:
            Mask = RCC_APB2ENR_IOPAEN;
            break;
        case GPIOB_BASE:
            Mask = RCC_APB2ENR_IOPBEN;
            break;
        case GPIOC_BASE:
            Mask = RCC_APB2ENR_IOPCEN;
            break;
        case GPIOD_BASE:
            Mask = RCC_APB2ENR_IOPDEN;
            break;
        case GPIOE_BASE:
            Mask = RCC_APB2ENR_IOPEEN;
            break;
    }
    return Mask;
}

void GPIO_EnablePort(GPIO_TypeDef *Port) {
    RCC->APB2ENR |= RCC_SelectMask(Port);
}

void GPIO_DisablePort(GPIO_TypeDef *Port) {
    RCC->APB2ENR &= ~RCC_SelectMask(Port);
}

void GPIO_InitPin(GPIO_TypeDef *Port, PINS PinNumber, PinOperations PinOperation){
    uint32_t Value;
    switch (PinOperation)
    {
    case PinOperationInputFloating:
        Value = 0x04;
        break;
    case PinOperationInputPullUpDown:
        Value = 0x08;
        break;
    case PinOperationOutputPushPull:
        Value = 0x03; //50MHZ Output
        break;
    case PinOperationOutputOpen:
        Value = 0x07;
        break;
    case PinOperationOutputAlternatePushPull:
        Value = 0x0B;
        break;
    case PinOperationOutputAlternateOpen:
        Value = 0x0F;
        break;

    case PinOperationAnalog:
    default:
        Value = 0;
        break;
    }

    if(PinNumber < PIN_08) {
        Port->CRL &= ~(0xF << (PinNumber * 4));
        Port->CRL |= (Value << (PinNumber * 4));
    } else {
        PinNumber -= 8;
        Port->CRH &= ~(0xF << (PinNumber * 4));
        Port->CRH |= (Value << (PinNumber * 4));
    }
    
}

void GPIO_WritePin(GPIO_TypeDef *Port, PINS PinNumber, PinValues PinValue){
    Port->ODR &= ~(1U << PinNumber);
    if(PinValue == PinHigh)
        Port->ODR |= (1U << PinNumber);
}

int GPIO_ReadPin(GPIO_TypeDef *Port, PINS PinNumber){
    return (Port->IDR & (1U << PinNumber)) >> PinNumber;
}

void GPIO_TogglePin(GPIO_TypeDef *Port, PINS PinNumber){
    Port->ODR ^= (1U << PinNumber);
}
