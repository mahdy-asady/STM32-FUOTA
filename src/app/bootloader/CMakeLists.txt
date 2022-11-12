cmake_minimum_required(VERSION 3.20)

if( MCU STREQUAL "cortex-m3" )
    add_definitions(-DHAL_GPIO_MODULE_ENABLED)

    set(INC_FILES main.c
        ../../mcu/stm32f107/GPIO.c
        ../../mcu/stm32f107/USART.c
        ../../mcu/stm32f107/flash.c
        ../../shared/fuota.c
        ../../shared/timer.c
        ../../shared/newstring.c
        ../../shared/ESP-AT.c
        ../../shared/fifo.c
        ../../mcu/stm32f107/startup_stm32f107xc.s
    )
endif()

# Project setup
add_executable(${PROJECT_NAME} ${INC_FILES})