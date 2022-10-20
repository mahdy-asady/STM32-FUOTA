#include <stdint.h>
#include "stm32f107xc.h"

int main(void) {

    //Config GPIO Pin D4 as Output
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    GPIOD->CRL = GPIO_CRL_MODE4_1;

    while(1) {
        GPIOD->ODR |= GPIO_ODR_ODR4;
        for (volatile int i = 0; i < 500000; i++);
        GPIOD->ODR &= ~GPIO_ODR_ODR4;
        for (volatile int i = 0; i < 500000; i++);
    }
}

void SystemInit(void) {

}