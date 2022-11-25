# STM32-FUOTA
## Introduction
IoT ([Internet of Things](https://en.wikipedia.org/wiki/Internet_of_things)) technology offers a lot of undeniable benefits to our lives. Nowadays, it's nearly impossible to find someone who doesn't own any devices that use this technology.

There are some upcoming needs coming out of the cloud, and what can we do with our devices to meet them? Yes. Reprogram them!
However, could you contact the buyers to ask them to bring the devices to the factory to be updated? The answer is simple: No!

So what's the most suitable solution? We call it **FUOTA**.

## What is FUOTA
**F**irmware **U**pdate **O**ver **T**he **A**ir is a solution to update application software running on the device. It uses a bootloader application to update the main application from the web. It can also backup the main application to a slave section, restore the backed up application version, or anything that should be done over the main application.

## What is a bootloader
In the microcontroller world, it is an application that runs upon powering on or resetting the microcontroller and updates or starts the main application.  Many microcontrollers, such as those from the STM32 series, have a built-in bootloader (AKA embedded bootloader) that allows firmware updates over USART, SPI, CAN, or even USB protocols. To update firmware using these protocols, a second device is required as a connector or a data container. Also the source code should be transferred to that device somehow!

On the other hand, the embedded bootloader has limited functionality. It simply updates the firmware and there is no way to determine if it did its job well or not!

For maximum performance, I decided to develop a lightweight bootloader that is robust and powerful. For now it can update, backup, restore, and of course start the main app. You should know that a custom bootloader is an ordinary application that has some specific options that will be discussed later.

The first yet most significant step is to figure out a way to start the custom bootloader and then the main app. For this purpose we must store both of them in the MCU's flash memory. And to do that, we need to partition the flash.

## Memory partitioning
As an example my MCU (STM32F107) has 256Kbytes of flash memory which can be programmed to run application codes! Well, is it possible to consider it as a file system and put two or even three files separately on it? I'm telling you that it's possible.

Let me explain how, but before that, you have to know some rules about flash memory operations.

### Flash access principles
- Read:
  Each section of flash memory can be read without limit (with some exceptions that are out of context of this topic), it can be read by 8, 16 or even 32 bits at a time.
- Write:
  It can be programmed 16 bits at a time. In addition, the address must be empty before any writing can take place.
- Erase:
  Flash has page-erasing functionality. It means that every attempt will erase a page of flash at once. On my MCU like most of the MCUs the page size is 2Kbytes (2048 bytes). During every erase operation, all bits will become 1.(Therefor a 32 bit cell will be 0xFFFFFFFF)

So we can partition flash memory in units of pages and put applications there. But how does the MCU decide which application should start? The answer is: it won't.

By default, MCU starts the application that sits on the right spot! And where is the right spot? It's the first word of flash memory that's mapped to address 0x08000000.

Whenever you partition flash memory, always keep in mind that the first partition must be the boot loader.

For some reason that will be transparent later, I divided this flash into 4 sections:
- Bootloader
- Configuration memory
- Main application
- Backup application

And you can see how this decision is reflected in the [linker scripts files](src/LinkerScripts)

## Start the app
So for now we know that the embedded bootloader will start our bootloader app as a regular application. Now the custom bootloader should find the location of the main app and start it.

During discussions on flash memory partitioning, we specified the start location and size of each section and wrote those details into the linker scripts. Now we need to access those information from linker script variables.
```C
extern char FLASH_APP1_OFFSET;
```
Variables in linker scripts are actually pointers to addresses. So when you redefine those variables in your code, keep in mind that the content of the variable is **not** relevant, the address of it is what we need!

Every application section begins with the stack pointer address in word length, followed by the address of the app's start point, known as the **Reset Handler**. So we should create a function reference and call it. But before calling the function we should set the vector table (For triggering interrupt handlers) and stack pointer:
```C
    void (*AppResetHandler)(void) = (void*)(*((volatile uint32_t*) (&FLASH_APP1_OFFSET + 4U)));
    //Set Vector Table
    SCB->VTOR = (uint32_t)&FLASH_APP1_OFFSET;
    // Set stack pointer
    __set_MSP((uint32_t)*((__IO uint32_t*)&FLASH_APP1_OFFSET));
    //Run Application's ResetHandler
    AppResetHandler();
```

## Connecting To The Internet
What all IoT devices have in common is the use of the Internet. There are several ways to connect to the internet, but for me, the cheapest option was to rely on an already purchased ESP32 development board. And how can I use the ESP32 features? Just use UART for communication and ESP-AT as the firmware on ESP32. You can find ESP-AT commands [here](https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/index.html).

### USART Challenge
It is known that when using the USART protocol as a communication protocol between two devices, the speed of the transfer of data may increase for a moment. You may miss some characters if you try to fetch them separately. The solution is to try using a list like FIFO! You can see a simple implementation in this project [here](src/shared/fifo.c)

## Intercommunication
Some times you need some communication between the custom bootloader and the main app. For example main app tries to tell the bootloader to update the app. In order to fulfill this need, I used **Backup Registers**. They are part of the RTC module of the MCU that should be activated before use. The implementation lies [here](src/mcu/stm32f107/bkpreg.c). By the way this variables only lives when power is on! If the MCU loses power, the register content will be lost.

## Flash Memory Modification
We have already discussed the rules for accessing and modifying flash memory. There is only one point to make. You have to unlock the flash memory before making any modifications.
```C
bool FlashUnlock(void) {
    if(FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
        if(FLASH->CR & FLASH_CR_LOCK)
            return false;
    }
    return true;
}
```

## Security & Integrity
There are some points that are crucial in any web communication:
- Identification of both sides of a communication
- Integrity of transferred data
- Securely & privately transfer data

The identification of a MCU is based on the device ID which is stored on it and can be achieved by reading some of its addresses. It is implemented [here](src/peripheral/devId.h).

The integrity of transferred data from the server is checked by CRC ([Cyclic Redundancy Check](https://en.wikipedia.org/wiki/Cyclic_redundancy_check)). In some MCUs including mine it is implemented in hardware as a module and can be activated easily. The implementation is [here](src/mcu/stm32f107/crc.c)

There are several methods for determining the identity of the server (that sends binary data to the device). I used cryptography for this purpose. At first, I implemented [ECC](https://en.wikipedia.org/wiki/Elliptic-curve_cryptography) encryption, which is asymmetric cryptography or [public key cryptography](https://en.wikipedia.org/wiki/Public-key_cryptography). But the overhead was higher than I expected! Therefore, I switched to [Symmetric Cryptography algorithms](https://en.wikipedia.org/wiki/Symmetric-key_algorithm). I chose [Noekeon](http://gro.noekeon.org/) because of its minimal overhead! Personally, I prefer asymmetric algorithms because the keys are different on both sides. This is because if anyone can access flash memory content, the stored key is ineffective for faking the server's identity.

## Storing the app's configuration
The **configuration memory** partition was one of the memory partitions mentioned previously. Based on [STM32 AN4894](https://www.st.com/resource/en/application_note/an4894-eeprom-emulation-techniques-and-software-for-stm32-microcontrollers-stmicroelectronics.pdf) you can emulate flash memory as EEPROM for storing variable information.