#ifndef SYS_INIT_H
#define SYS_INIT_H

#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/systick.h>
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t millis();
uint64_t micros();
void sys_tick_handler();
void delay(uint64_t duration);
void delay_us(uint64_t duration);
void clock_setup();
void i2c_setup();
void usart_setup();
void systick_setup();
void gpio_setup();

#ifdef __cplusplus
}
#endif

#endif /* SYS_INIT_H */
