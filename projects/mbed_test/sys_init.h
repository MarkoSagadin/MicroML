#ifndef SYS_INIT_H
#define SYS_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern volatile uint8_t g_clock_mhz; //defined in sys_init.c
void clock_setup();
void i2c_setup();
void spi_setup();
void usart_setup();
void systick_setup();
void gpio_setup();

#ifdef __cplusplus
}
#endif

#endif /* SYS_INIT_H */
