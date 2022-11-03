#include "string.h"
#include "USART.h"
#include "delay.h"
#include "debug.h"
#include "config.h"

USART_Handle *Connection, *Echo;

int SendCommandAndWait(char *cmd, uint32_t Timeout) {
    USART_WriteLine(Connection, cmd);
    
    char strBuffer[100];
    int Result = 0;

    uint32_t Holder = SetTimeout(Timeout);
    while (!TimeoutReached(&Holder))
    {
        Result = USART_ReadLine(Connection, strBuffer, 100);
        if(strcmp(strBuffer, "ERROR") == 0)
            return 0;
        if(strcmp(strBuffer, "OK") == 0)
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
    strconcat(ConnectionString, 100, 5, "AT+CWJAP=\"", SSID, "\",\"", Password, "\"");
    log_info(Echo, ConnectionString);
    if(!SendCommandAndWait(ConnectionString, AP_CONNECTION_DELAY))
        log_info(Echo, "Access Point Connection failed!!!");
    else
        log_info(Echo, "Access Point Connected!!!");

    return 1;
}

int ESP_GetURL(char *URI, char *buffer, uint8_t MaxLength) {
    return 0;
}