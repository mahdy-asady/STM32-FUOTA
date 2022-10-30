#ifndef _ESP_AT_H_
#define _ESP_AT_H_

void ESP_Init(USART_Handle*, USART_Handle*);
int ESP_WifiConnect(char *SSID, char *Password);
int ESP_GetURL(char *URI, char *buffer, uint8_t MaxLength);

#endif