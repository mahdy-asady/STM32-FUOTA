cmake_minimum_required(VERSION 3.20)

if( MCU STREQUAL "cortex-m3" )
    add_definitions(-DHAL_GPIO_MODULE_ENABLED)

    set(INC_FILES main.c
        ../../mcu/stm32f107/GPIO.c
        ../../mcu/stm32f107/USART.c
        ../../mcu/stm32f107/eeprom.c
        ../../mcu/stm32f107/timer.c
        ../../mcu/stm32f107/flash.c
        ../../mcu/stm32f107/crc.c
        ../../mcu/stm32f107/board.c
        ../../mcu/stm32f107/bkpreg.c
        ../../mcu/stm32f107/devId.c
        ../../shared/fuota.c
        ../../shared/newstring.c
        ../../shared/ESP-AT.c
        ../../shared/fifo.c
        ../../mcu/stm32f107/startup_stm32f107xc.s
        ../../shared/Noekeon/Noekeon.c
    )
endif()

# Project setup
add_executable(${PROJECT_NAME} ${INC_FILES})