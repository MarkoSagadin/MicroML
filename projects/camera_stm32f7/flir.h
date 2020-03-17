#ifndef FLIR_H
#define FLIR_H

#include <stdbool.h>
#include "flir_defines.h"
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLIR_BUSY_TIMEOUT (5000)

// Low level commands
bool get_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words);
bool set_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words);

uint16_t command_code(uint16_t cmd_id, uint16_t cmd_type);
bool wait_busy_bit(uint16_t timeout);
LEP_RESULT get_last_flir_result();

//I2C commands, i2c peripheral should be initialized before using flir.h
static bool write_register(uint16_t reg_address, uint16_t value);
static bool read_register(uint16_t reg_address, uint16_t * value);
//static bool write_command_register(uint16_t cmd_code, uint16_t * data_words, uint16_t num_words);
static bool read_data_register(uint16_t * read_words, uint8_t max_length);

// Utility functions, hardware depended
static void flir_delay(uint32_t delay);
static uint32_t flir_millis();

#ifdef __cplusplus
}
#endif

#endif
/*** end of file ***/
