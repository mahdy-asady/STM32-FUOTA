cmake_minimum_required(VERSION 3.20)

set(PROJECT_PATH "src/app/${PROJECT}")

set(CDEFS     "-DF_CPU=${F_CPU} -DBAUD=${BAUD} -D${PROCESSOR} -DWIFI_SSID=\\\"${WIFI_SSID}\\\" -DWIFI_PASS=\\\"${WIFI_PASS}\\\" -DUPDATE_SERVER=\\\"${UPDATE_SERVER}\\\"")
set(CSTANDARD "-std=gnu99")
set(CWARN     "-Wall -Wextra -Wstrict-prototypes -Wshadow -Wdouble-promotion -Wundef -fno-common -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable")

# Cmake test use nosys.spec for testing arm-none-eabi-gcc
set(CMAKE_EXE_LINKER_FLAGS_INIT "-specs=nosys.specs -specs=nano.specs")

set(PREFIX arm-none-eabi-)
set(CTUNING   "-fno-builtin -ffunction-sections -fdata-sections")
set(CMCU      "-mcpu=${MCU} -mthumb -DUSE_HAL_DRIVER")

if( BUILD STREQUAL "release")

    set(CDEBUG    "-fanalyzer -DLOG_LEVEL=0")
    set(COPT      "-Os")

elseif( BUILD STREQUAL "debug")

    set(CDEBUG    "-Og -g3 -fanalyzer -DLOG_LEVEL=${LOG_LEVEL}")
    set(COPT      "")

endif()

if(PROJECT STREQUAL "bootloader")

    set( LINKER_SCRIPT "STM32F107VCTx_FLASH_BOOTLOADER.ld")
    set( Flash_ADDR "0x8000000" )

else()
    set( LINKER_SCRIPT "STM32F107VCTx_FLASH_APPLICATION.ld")
    set( Flash_ADDR "0x8001800" )

endif()

set(CFLAGS   "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CXXFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CTUNING}")

# program names
set(CPP   ${PREFIX}g++)
set(CC     ${PREFIX}gcc)
set(STRIP ${PREFIX}strip)
set(OBJCOPY  ${PREFIX}objcopy)
set(OBJDUMP  ${PREFIX}objdump)
set(SIZE  ${PREFIX}size)



# Sets the compiler
# Needs to come before the project function
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_CXX_COMPILER ${CPP})
set(CMAKE_C_COMPILER   ${CC})
set(CMAKE_ASM_COMPILER ${CC})

set(CMAKE_C_FLAGS   "${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")
set(CMAKE_ASM_FLAGS   "${CFLAGS}")

project(Embedded C ASM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# Project setup
add_subdirectory(${PROJECT_PATH})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

target_include_directories(${PROJECT_NAME} PUBLIC "src/peripheral")

target_link_options(${PROJECT_NAME} PRIVATE
    -T../../../../src/LinkerScripts/${LINKER_SCRIPT}
    -mcpu=cortex-m3
    -mthumb
    -lc
    -lm
    -lc_nano
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -Wl,--relax
)

add_custom_target(binary   ALL     ${OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin && ${SIZE} --format=berkeley ${PROJECT_NAME}.elf DEPENDS ${PROJECT_NAME})

add_custom_target(flash st-flash write ${PROJECT_NAME}.bin ${Flash_ADDR} && st-flash reset DEPENDS binary)


target_include_directories(${PROJECT_NAME} PUBLIC "src/mcu/stm32f107")
target_include_directories(${PROJECT_NAME} PUBLIC "src/shared")
target_include_directories(${PROJECT_NAME} PUBLIC "src/mcu/stm32f107/CMSIS/Include")

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")
