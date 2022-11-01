#include <string.h>
//#include <stdio.h>
#include "USART.h"
#include "delay.h"
#include "debug.h"

USART_Handle *Connection, *Echo;

int SendCommandAndWait(char *cmd, uint32_t Delay) {
    USART_WriteLine(Connection, cmd);
    
    char strBuffer[100];
    int Result = 0;

    DelaySet(Delay);
    while (!DelayCatched())
    {
        Result = USART_ReadLine(Connection, strBuffer, 100);
        if(!Result || strcmp(strBuffer, "ERROR") == 0)
            return 0;
        if(strcmp(strBuffer, "OK") == 0)
            return 1;
    }
    return 0;
}

#define SendCommand(CMD) SendCommandAndWait(CMD, 200)


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
    char ConnectionString[100] = "AT+CWJAP=\"Redmi\",\"00000000\"";
    //snprintf(ConnectionString, 100, "AT+CWJAP=\"%s\",\"%s\"", SSID, Password);
    if(!SendCommandAndWait(ConnectionString, 1000))
        log_info(Echo, "Access Point Connection failed!!!");
    else
        log_info(Echo, "Access Point Connected!!!");

    return 1;
}

int ESP_GetURL(char *URI, char *buffer, uint8_t MaxLength) {
    return 0;
}