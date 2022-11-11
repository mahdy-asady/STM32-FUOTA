cmake_minimum_required(VERSION 3.20)


set(INC_FILES main.c
    ../../mcu/stm32f107/GPIO.c
    ../../mcu/stm32f107/USART.c
    ../../mcu/stm32f107/startup_stm32f107xc.s
)

# Project setup
add_executable(${PROJECT_NAME} ${INC_FILES})