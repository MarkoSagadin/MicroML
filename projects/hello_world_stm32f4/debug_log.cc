#include "tensorflow/lite/micro/debug_log.h"
#include <libopencm3/stm32/usart.h>
#include <string.h>
//#include "printf.h"

extern "C" void DebugLog(const char* s) 
{
  // Simpler version of printing to serial, we can use what
  // Tensorflow created.
  //printf("%s", s);
  for (size_t i = 0, j = strlen(s); i < j; i++) 
  {
      usart_send_blocking(USART6, s[i]);
  }
}

