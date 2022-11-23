#ifndef __CONFIG_H_
#define __CONFIG_H_

#define FIFO_BUFFER_SIZE 500

#define EMPTY_BUFFER_WAIT_DELAY 100

#define DEFAULT_RESPONSE_DELAY 200

#define AP_CONNECTION_DELAY 5000

#define HTTP_TIMEOUT 5000

#define HTTP_DOWNLOAD_RETRY_MAX 5

#define HTTP_RETRY_WAIT_TIME 3000

#define RESPONSE_LINE_BUFFER_LENGTH 100

/* We use Noekeon as cipher and its block length is 128 bit(16 bytes). So downloaded chunks should be dividable by 16 */
#define DOWNLOAD_CHUNK_SIZE 128

enum EEPROM_VARS {
    App1Size = 0x00000001UL,
    App1Version,
    App2Size,
    App2Version
};

#endif