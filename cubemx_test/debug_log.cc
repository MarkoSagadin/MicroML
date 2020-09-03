#include <tensorflow/lite/micro/debug_log.h>
#include <string.h>
#include "main.h"

extern UART_HandleTypeDef huart3;

extern "C" void DebugLog(const char* s) 
{
    for (size_t i = 0, j = strlen(s); i < j; i++) 
    {
        uint8_t c = (uint8_t)s[i];
        HAL_UART_Transmit(&huart3, &c, 1, 0xffffff);
    }
}


