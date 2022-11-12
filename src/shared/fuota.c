#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "config.h"
#include "USART.h"
#include "ESP-AT.h"
#include "newstring.h"
#include "debug.h"
#include "flash.h"


extern USART_Handle UsartDebug;
extern char FLASH_APP1_OFFSET;


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

bool GetUpdateInfo(uint32_t *FileVersion, char *FileName, uint32_t *FileSize, uint8_t *FileCRC) {
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

    *FileCRC = *((uint32_t*)FileContentBuffer + AddressOffset);

    return true;
}

bool DownloadUpdate(char *FilePath, uint32_t FileSize) {
    uint32_t StartOffset = 0;
    uint32_t EndOffset = DOWNLOAD_CHUNK_SIZE - 1;
    uint8_t FileContentBuffer[DOWNLOAD_CHUNK_SIZE];
   
   if(!FlashUnlock())
        return false;
    
    FlashErase((uint32_t)&FLASH_APP1_OFFSET, FileSize);

    while(StartOffset < FileSize) {
        uint8_t ContentSize = ESP_GetFileChunk(FilePath, StartOffset, EndOffset, FileContentBuffer, DOWNLOAD_CHUNK_SIZE);
        if(ContentSize == 0)
            return false;

        FlashWrite((uint32_t)&FLASH_APP1_OFFSET + StartOffset, FileContentBuffer, ContentSize);

        char StrOffset[20];
        Num2Str(StartOffset, StrOffset);
        log_info(&UsartDebug, StrOffset);
        
        StartOffset += DOWNLOAD_CHUNK_SIZE;
        EndOffset += DOWNLOAD_CHUNK_SIZE;
    }
    FlashLock();
    return true;
}

void FUOTA_Update(void) {
    uint32_t    UpdateVersion;
    char        FileName[255];
    uint32_t    FileSize;
    uint8_t     FileCRC[4];
    GetUpdateInfo(&UpdateVersion, FileName, &FileSize, FileCRC);

    char BinaryFileName[255];
    StrConcat(BinaryFileName, 255, 3, UPDATE_SERVER, "/", FileName);
    
    DownloadUpdate(BinaryFileName, FileSize);
}
