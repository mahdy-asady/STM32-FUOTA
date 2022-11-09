//#include <stdlib.h>
#include "newstring.h"
#include "USART.h"
#include "timer.h"
#include "debug.h"
#include "config.h"

USART_Handle *Connection, *Echo;

int SendCommandAndWait(char *cmd, uint32_t Timeout) {
    USART_WriteLine(Connection, cmd);
    char strBuffer[RESPONSE_LINE_BUFFER_LENGTH];

    uint16_t Holder = GetSysTick();
    while (!TimeoutReached(Holder, Timeout))
    {
        USART_ReadLine(Connection, strBuffer, RESPONSE_LINE_BUFFER_LENGTH);

        #ifdef DEBUG_TO_USART
        log_info(Echo, strBuffer);
        #endif
        if(StrCompare(strBuffer, "ERROR") == 0)
            return 0;
        if(StrCompare(strBuffer, "OK") == 0)
            return 1;
    }
    return 0;
}

#define SendCommand(CMD) SendCommandAndWait(CMD, DEFAULT_RESPONSE_DELAY)


void ESP_Init(USART_Handle *ESP_Connection, USART_Handle *Echo_Connection) {
    Connection = ESP_Connection;
    Echo = Echo_Connection;

    log_info(Echo, "Initialize ESP-AT ...");

    if(!SendCommand("AT"))
        log_info(Echo, "ESP-AT Initialization failed!!!");

    SendCommand("ATE0");
}

int ESP_WifiConnect(char *SSID, char *Password) {
    log_info(Echo, "Set station Mode ...");
    SendCommand("AT+CWMODE=1");//Set mode to wifi station
    
    log_info(Echo, "Connect to Access point ...");
    char ConnectionString[100];
    StrConcat(ConnectionString, 100, 5, "AT+CWJAP=\"", SSID, "\",\"", Password, "\"");
    log_info(Echo, ConnectionString);
    if(!SendCommandAndWait(ConnectionString, AP_CONNECTION_DELAY))
        log_info(Echo, "Access Point Connection failed!!!");
    else
        log_info(Echo, "Access Point Connected!!!");

    return 1;
}

//fetches the range, puts to buffer, return the length
uint8_t ESP_GetFileChunk(char *URI, uint32_t StartByte, uint32_t EndByte, uint8_t *Buffer, uint8_t MaxLength) {
    if((EndByte - StartByte + 1) > MaxLength)
        return 0;
    
    char CommandText[200];
    uint8_t TempCharacter;
    uint8_t ChunkSize = 0;

    char StartRange[11], EndRange[11];
    Num2Str(StartByte, StartRange);
    Num2Str(EndByte, EndRange);

    StrConcat(CommandText, 200, 7, "AT+HTTPCLIENT=2,0,\"", URI, "\",,,1,\"Range: bytes=", StartRange, "-", EndRange, "\"");
    USART_WriteLine(Connection, CommandText);

    if(!USART_BufferPopWithTimeout(Connection, &TempCharacter, HTTP_TIMEOUT) || TempCharacter != '+')
        return 0;

    //remove "HTTPCLIENT:" From Buffer
    for(uint8_t i = 0; i< 11; i++)
        USART_BufferPopWithTimeout(Connection, &TempCharacter, HTTP_TIMEOUT);

    //get chunk size
    TempCharacter = '0';
    while(TempCharacter >= '0' && TempCharacter <= '9') {
        ChunkSize = ChunkSize * 10 + (TempCharacter - '0');
        USART_BufferPopWithTimeout(Connection, &TempCharacter, HTTP_TIMEOUT);
    }
    //Get Content
    for(uint8_t i = 0; i< ChunkSize; i++)
        USART_BufferPopWithTimeout(Connection, (uint8_t *)&Buffer[i], HTTP_TIMEOUT);
    
    //Content Finish, read untill end of content
    while(USART_BufferPopWithTimeout(Connection, &TempCharacter, 50));

    return ChunkSize;
}
