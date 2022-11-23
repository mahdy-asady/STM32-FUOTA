#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "fuota.h"
#include "config.h"
#include "USART.h"
#include "ESP-AT.h"
#include "newstring.h"
#include "debug.h"
#include "flash.h"
#include "crc.h"
#include "eeprom.h"
#include "timer.h"

#include "Noekeon/NessieInterfaces.h"
#include "Noekeon/Nessie.h"


extern USART_Handle UsartDebug;
extern uint8_t FLASH_APP1_OFFSET;
extern uint8_t FLASH_APP2_OFFSET;


/*
    update file data structure:
    Index   Byte Length     Content
    0       1               Version Patch
    1       1               Version Minor
    2       1               Version Major
    3       1               NULL
    4       1               File Name Length
    5       FileLength      File Name
    6       1               NULL
    7       4               File Size (In little Endian 32 bit number)
    8       4               File Content CRC32
    9       1               NULL

*/

bool GetUpdateInfo(uint32_t *FileVersion, char *FileName, uint32_t *FileSize, uint32_t *FileCRC) {
    uint8_t FileContentBuffer[DOWNLOAD_CHUNK_SIZE];
   
    uint8_t ContentSize = ESP_GetFileChunk(UPDATE_SERVER "/update", 0, DOWNLOAD_CHUNK_SIZE - 1, FileContentBuffer, DOWNLOAD_CHUNK_SIZE);
    if(!ContentSize)
        return false;
    
    uint8_t AddressOffset = 0;

    *FileVersion = *((uint32_t*)FileContentBuffer);
    AddressOffset += 4;

    uint8_t FileNameLength = *((uint8_t*)(FileContentBuffer + AddressOffset));
    AddressOffset += 1;

    strlcpy(FileName, (char *)(FileContentBuffer + AddressOffset), FileNameLength + 1);
    AddressOffset += FileNameLength + 1;

    *FileSize = *((uint32_t*)(FileContentBuffer + AddressOffset));
    AddressOffset += 4;

    *FileCRC = *((uint32_t*)(FileContentBuffer + AddressOffset));

    return true;
}

bool DownloadUpdate(char *FilePath, uint32_t FileSize) {
    uint32_t StartOffset = 0;
    uint32_t EndOffset = DOWNLOAD_CHUNK_SIZE - 1;
    uint8_t DownloadContentBuffer[DOWNLOAD_CHUNK_SIZE],
            DecryptedContentBuffer[DOWNLOAD_CHUNK_SIZE];

    uint8_t Key[16] = { 0x27, 0x94, 0x36, 0x00, 0x78, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    struct NESSIEstruct KeyStruct;
    NESSIEkeysetup (Key, &KeyStruct);
   
   if(!FlashUnlock())
        return false;
    
    FlashErase((uint32_t)&FLASH_APP1_OFFSET, FileSize);

    while(StartOffset < FileSize) {
        uint8_t Retry = 0;
        uint8_t ContentSize;
        do{
            ContentSize = ESP_GetFileChunk(FilePath, StartOffset, EndOffset, DownloadContentBuffer, DOWNLOAD_CHUNK_SIZE);
            if(!ContentSize) {
                if(++Retry > HTTP_DOWNLOAD_RETRY_MAX)
                    return false;
                Delay_ms(HTTP_RETRY_WAIT_TIME);
            }
        } while (ContentSize == 0);

        for(uint32_t i = 0; i < DOWNLOAD_CHUNK_SIZE / 16; i++) {
            uint32_t Gap = i * 16;
            NESSIEdecrypt (&KeyStruct, DownloadContentBuffer + Gap, DecryptedContentBuffer + Gap);
        }
        

        FlashWrite((uint32_t)&FLASH_APP1_OFFSET + StartOffset, DecryptedContentBuffer, ContentSize);

        char StrOffset[20];
        Num2Str(StartOffset, StrOffset);
        log_info(&UsartDebug, StrOffset);
        
        StartOffset += DOWNLOAD_CHUNK_SIZE;
        EndOffset += DOWNLOAD_CHUNK_SIZE;
    }
    FlashLock();
    return true;
}

void fuotaUpdate(void) {
    uint32_t    UpdateVersion;
    char        FileName[255];
    uint32_t    FileSize;
    uint32_t     FileCRC;
    GetUpdateInfo(&UpdateVersion, FileName, &FileSize, &FileCRC);

    char BinaryFileName[255];
    StrConcat(BinaryFileName, 255, 3, UPDATE_SERVER, "/", FileName);
    
    if(!DownloadUpdate(BinaryFileName, FileSize)) {
        log_error(&UsartDebug, "File download failed!");

        log_error(&UsartDebug, "Restoring Backup!");
        fuotaRestore();
        
        return;
    }
    
    uint32_t WordCount = FileSize / 4 + ((FileSize % 4) > 0);
    uint32_t WriteCRC = CRC_Calculate((uint32_t *)&FLASH_APP1_OFFSET, WordCount);
    
    if(WriteCRC != FileCRC) {
        char Number[20];

        log_error(&UsartDebug, "File verification failed!");
        
        log_info(&UsartDebug, "Server CRC:");
        Num2Str(FileCRC, Number);
        log_info(&UsartDebug, Number);

        log_info(&UsartDebug, "Micro CRC:");
        Num2Str(WriteCRC, Number);
        log_info(&UsartDebug, Number);

        log_error(&UsartDebug, "Restoring Backup!");
        fuotaRestore();

        return;
    }
    
    EE_Write(App1Size, FileSize);
    EE_Write(App1Version, UpdateVersion);
}

void FlashCopy(uint8_t *SrcAddress, uint32_t DstAddress, uint32_t Length) {
    if(!FlashUnlock()) {
        return;
    }

    FlashErase(DstAddress, Length);
    FlashWrite(DstAddress, SrcAddress, Length);

    FlashLock();
}

void fuotaBackup(void) {
    uint32_t FileSize = EE_Read(App1Size, 0), 
             FileVersion = EE_Read(App1Version, 0);

    FlashCopy(&FLASH_APP1_OFFSET, (uint32_t)&FLASH_APP2_OFFSET, FileSize);
    
    EE_Write(App2Size, FileSize);
    EE_Write(App2Version, FileVersion);
}

void fuotaRestore(void) {
    uint32_t FileSize = EE_Read(App2Size, 0), 
             FileVersion = EE_Read(App2Version, 0);

    FlashCopy(&FLASH_APP2_OFFSET, (uint32_t)&FLASH_APP1_OFFSET, FileSize);

    EE_Write(App1Size, FileSize);
    EE_Write(App1Version, FileVersion);
}
