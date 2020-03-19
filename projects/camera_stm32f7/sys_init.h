#ifndef SYS_INIT_H
#define SYS_INIT_H

#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/cm3/systick.h>
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

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
