#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>

#define NONE                0
#define ERROR_LEVEL         1
#define WARNING_LEVEL       2
#define INFO_LEVEL          3
#define TRACE_LEVEL         4

/*      TRACE   INFO    WARN    ERROR   NONE
TRACE   YES      NO      NO      NO      NO
INFO    YES      YES      NO      NO      NO
WARN    YES      YES      YES      NO      NO
ERROR   YES      YES      YES      YES      NO*/

/* -- Macro Definitions */

#if LOG_LEVEL>= ERROR_LEVEL
    #define num2str(x) str(x)
    #define str(x) #x


    #define log_error(USART, M)         ({USART_SendString(USART, "[ERR] (" __BASE_FILE__ ":" num2str(__LINE__) ") "); USART_WriteLine(USART, M);})
#else
    #define log_error(USART, M)         do {} while(0)
#endif

#if LOG_LEVEL>= WARNING_LEVEL
    #define log_warning(USART, M)       ({USART_SendString(USART, "[WARN] (" __BASE_FILE__ ":" num2str(__LINE__) ") "); USART_WriteLine(USART, M);})
#else
    #define log_warning(USART, M)       do {} while(0)
#endif

#if LOG_LEVEL>= INFO_LEVEL
    #define log_info(USART, M)          ({USART_SendString(USART, "[INFO] (" __BASE_FILE__ ":" num2str(__LINE__) ") "); USART_WriteLine(USART, M);})
    #define DebugBlock(x)               do {x} while(0)
#else
    #define log_info(USART, M)          do {} while(0)
    #define DebugBlock(x)               do {} while(0)
#endif

#if LOG_LEVEL>= TRACE_LEVEL
    #define log_trace(USART)            USART_WriteLine(USART, "[..] (" __BASE_FILE__ ":" num2str(__LINE__) "->" __func__)
#else
    #define log_trace(USART)            do {} while(0)
#endif

#endif /*__DEBUG_H_*/
