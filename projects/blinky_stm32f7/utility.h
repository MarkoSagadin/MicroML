#ifndef utility_H
#define utility_H

#include <libopencm3/cm3/systick.h>

#ifdef __cplusplus
extern "C" {
#endif

// Time delay related functions, systick_setup is in sys_init.c
uint64_t millis();
uint64_t micros();
void sys_tick_handler();
void delay(uint64_t duration);
void delay_us(uint64_t duration);

#ifdef __cplusplus
}
#endif

#endif /* utility_H */
/*** end of file ***/
