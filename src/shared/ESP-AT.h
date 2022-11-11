#ifndef _ESP_AT_H_
#define _ESP_AT_H_

void ESP_Init(USART_Handle*, USART_Handle*);
int ESP_WifiConnect(char *SSID, char *Password);
uint8_t ESP_GetFileChunk(char *URI, uint32_t StartByte, uint32_t EndByte, uint8_t *Buffer, uint8_t MaxLength);

#endif