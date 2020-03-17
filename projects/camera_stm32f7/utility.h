#ifndef utility_H
#define utility_H

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/systick.h>
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_TIMEOUT 1000    // In ms
#define I2C_READ_DIR    0
#define I2C_WRITE_DIR   1

// I2C related functions
void i2c_prepare(uint32_t i2c, uint8_t addr, uint8_t dir, uint8_t num_bytes);

bool i2c_write(uint8_t addr, uint8_t data);
bool i2c_write_array(uint8_t addr, uint8_t * data, uint8_t num_bytes);
bool i2c_write16(uint8_t addr, uint16_t data);
bool i2c_write16_array(uint8_t addr, uint16_t * data, uint8_t num_words);

bool i2c_read(uint8_t addr, uint8_t * data);
bool i2c_read16(uint8_t addr, uint16_t * data);
bool i2c_read_array(uint8_t addr, uint8_t * data, uint8_t num_bytes);
bool i2c_read16_array(uint8_t addr, uint16_t * data, uint8_t num_words);

bool wait_for_ack(uint32_t timeout);
bool wait_for_empty_data_reg(uint32_t timeout);

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
