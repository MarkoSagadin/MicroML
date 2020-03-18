#ifndef FLIR_H
#define FLIR_H

#include <stdbool.h>
#include "flir_defines.h"
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLIR_BUSY_TIMEOUT (5000)

// Debug macro, define it to enable debug uart
#define FLIR_DEBUG

// Low level commands
bool get_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words);
bool set_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words);

uint16_t command_code(uint16_t cmd_id, uint16_t cmd_type);
bool wait_busy_bit(uint16_t timeout);
LEP_RESULT get_last_flir_result();

#ifdef __cplusplus
}
#endif

#endif
/*** end of file ***/
