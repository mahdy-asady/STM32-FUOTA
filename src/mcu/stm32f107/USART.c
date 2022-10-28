#include "stm32f107xc.h"
#include "GPIO.h"

void (*USART1CallBack)(char);
void (*USART2CallBack)(char);

void USART_EnableUSART1(void) {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    GPIO_EnablePort(GPIOA);
    GPIO_InitPin(GPIOA, PIN_09, PinOperationOutputAlternatePushPull);
    GPIO_InitPin(GPIOA, PIN_10, PinOperationInputPullUpDown);
    
    USART1->BRR = 40 << 4; // Baud rate = 72Mhz/(16 * __40__) = 112500
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void USART_FetchUSART1(void (*CallBack)(char)) {
    USART1CallBack = CallBack;
    __NVIC_EnableIRQ(USART1_IRQn);
    USART1->CR1 |= USART_CR1_RXNEIE;
}


void USART_EnableUSART2(void) {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;

    GPIO_EnablePort(GPIOD);
    GPIO_InitPin(GPIOD, PIN_05, PinOperationOutputAlternatePushPull);
    GPIO_InitPin(GPIOD, PIN_06, PinOperationInputPullUpDown);
    
    USART2->BRR = 20 << 4; // Baud rate = 36Mhz/(16 * __20__) = 112500
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void USART_FetchUSART2(void (*CallBack)(char)) {
    USART2CallBack = CallBack;
    __NVIC_EnableIRQ(USART2_IRQn);
    USART2->CR1 |= USART_CR1_RXNEIE;
}


void USART_SendString(USART_TypeDef *USART, char *Text) {
    while(*Text != 0) {
        while(!(USART->SR & USART_SR_TC));
        USART->DR = *Text;
        Text++;
    }
}


void USART1_IRQHandler (void) {
    char Data = (char)((uint8_t)(USART1->DR) & 0x7F);
    USART1CallBack(Data);
}


void USART2_IRQHandler (void) {
    char Data = (char)((uint8_t)(USART2->DR) & 0x7F);
    USART2CallBack(Data);
}