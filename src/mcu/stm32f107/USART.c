#include "stm32f107xc.h"
#include "GPIO.h"
#include "USART.h"
#include "delay.h"
#include "config.h"

USART_Handle *UHandles[2];


/*
    USART_Init
*/
void USART_Init(USART_Handle *USART) {
    //Initiate RX Buffer
    USART->Buffer.Start = USART->Buffer.End = USART->Buffer.Content;

    // Enable Alternate Function Clock
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    //Some configs based on USART Selection
    uint32_t USARTAddress = (uint32_t)USART->Instance;
    switch (USARTAddress)
    {
    case USART1_BASE:
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

        if(!USART->isPortMapped) {
            GPIO_EnablePort(GPIOA);
            GPIO_InitPin(GPIOA, PIN_09, PinOperationOutputAlternatePushPull);
            GPIO_InitPin(GPIOA, PIN_10, PinOperationInputPullUpDown);
        }
        else {
            AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
            //...
        }

        __NVIC_EnableIRQ(USART1_IRQn);
        UHandles[0] = USART;
        break;
    case USART2_BASE:
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

        if(!USART->isPortMapped) {
        }
        else {
            AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;

            GPIO_EnablePort(GPIOD);
            GPIO_InitPin(GPIOD, PIN_05, PinOperationOutputAlternatePushPull);
            GPIO_InitPin(GPIOD, PIN_06, PinOperationInputPullUpDown);
        }

        __NVIC_EnableIRQ(USART2_IRQn);
        UHandles[1] = USART;
        break;
    }
    
    USART->Instance->BRR = USART->BaudRate; // Baud rate = Clock(Mhz)/(16 * __XXX__)
    //Enable USART, RX, TX, TX Interrupt
    USART->Instance->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
}

/*
    USART_SendString
*/
void USART_SendString(USART_Handle *USART, char *Text) {
    while(*Text != 0) {
        while(!(USART->Instance->SR & USART_SR_TXE));
        USART->Instance->DR = *Text;
        Text++;
    }
}

/*
    USART_WriteLine
*/
void USART_WriteLine(USART_Handle *USART, char *Text) {
    USART_SendString(USART, Text);
    USART_SendString(USART, "\r\n");
}

/*
    USART_ReadLine
*/
int USART_ReadLine(USART_Handle *USART, char *ReturnString, uint8_t ReturnMaxSize){
    uint8_t StringPos = 0;
    char CurrentChar = 0;
    int Retry = 0;
    do{
        if(USART->Buffer.Start == USART->Buffer.End){ //Buffer Empty, wait for incomming
            if(Retry > EMPTY_BUFFER_RETRY_COUNT){
                ReturnString[StringPos] = 0;
                return 0;
            }
            else {
                Delay_ms(EMPTY_BUFFER_WAIT_DELAY);
                Retry++;
                continue;
            }
        }
        Retry = 0;
        CurrentChar = *(USART->Buffer.Start++);
        ReturnString[StringPos++] = CurrentChar;
        if(USART->Buffer.Start == (USART->Buffer.Content + USART_BUFFER_SIZE))
            USART->Buffer.Start = USART->Buffer.Content;

    }while(CurrentChar != '\n' && StringPos < ReturnMaxSize); // got \n or ReturnStriing full
    /*
        TODO: In Case of ReturnString full we will miss a character!!
    */

    //remove trailing \r\n
    ReturnString[StringPos - 1] = 0;
    if(ReturnString[StringPos-2] == '\r')
        ReturnString[StringPos - 2] = 0;
    return 1;
}

/*
    ReceiveData
*/
void ReceiveData(USART_Handle *USART) {
    while(USART->Instance->SR & USART_SR_RXNE) {
        char *NewEnd;
        if(USART->Buffer.End == (USART->Buffer.Content + USART_BUFFER_SIZE - 1)) {
            NewEnd = USART->Buffer.Content;
        }
        else {
            NewEnd = USART->Buffer.End + 1;
        }

        if(NewEnd == USART->Buffer.Start) {
            //Buffer overflow
            //we will miss some characters on buffer overflow
            char TempChar = (char)((uint8_t)(USART->Instance->DR) & 0x7F);
            return;
        }
        char Char = (char)((uint8_t)(USART->Instance->DR) & 0x7F);
        *(USART->Buffer.End) = Char;
        USART->Buffer.End = NewEnd;
    }
}


/************************************************************/

/*
    USART1_IRQHandler
*/
void USART1_IRQHandler (void) {
    ReceiveData(UHandles[0]);
}

/*
    USART2_IRQHandler
*/
void USART2_IRQHandler (void) {
    ReceiveData(UHandles[1]);
}