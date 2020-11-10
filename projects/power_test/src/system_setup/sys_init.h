#ifndef SYS_INIT_H
#define SYS_INIT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint8_t g_clock_mhz; //defined in sys_init.c

//#define SYSTICK_TIMER

void clock_setup();
void i2c_setup();
void spi_setup();
void usart_setup();
void systick_setup();
void dwt_setup();
void gpio_setup();
void system_setup();

#ifdef __cplusplus
}
#endif

#endif /* SYS_INIT_H */
