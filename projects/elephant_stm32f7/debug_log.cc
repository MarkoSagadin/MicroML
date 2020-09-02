#include "tensorflow/lite/micro/debug_log.h"
#include <libopencm3/stm32/usart.h>
#include <string.h>

extern "C" void DebugLog(const char* s) 
{
  for (size_t i = 0, j = strlen(s); i < j; i++) 
  {
      usart_send_blocking(USART3, s[i]);
  }
}


