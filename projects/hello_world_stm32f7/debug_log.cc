#include "tensorflow/lite/micro/debug_log.h"
#include "printf.h"

extern "C" void DebugLog(const char* s) {
  printf("%s", s);
}
