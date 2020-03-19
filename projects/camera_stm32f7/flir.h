#ifndef FLIR_H
#define FLIR_H

#include <stdbool.h>
#include "flir_defines.h"
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLIR_BUSY_TIMEOUT (5000)

// Debug options
// FLIR_DEBUG macro is used for debugging purposes, tracing path of functions
// and other problems. It is normally undefined
// FLIR_PRINT is used to display some data about FLIR, like serial number,
// current settings, etc. It is normally defined
#define FLIR_DEBUG
#define FLIR_PRINT

//General diagnostic, set and get functions
void display_flir_serial();
LEP_SYS_SHUTTER_POSITION get_flir_shutter_position();
void set_flir_shutter_position(LEP_SYS_SHUTTER_POSITION position);

// Low level commands
bool get_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words);
bool set_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words);

uint16_t command_code(uint16_t cmd_id, uint16_t cmd_type);
LEP_RESULT get_last_flir_result();

#ifdef __cplusplus
}
#endif

#endif
/*** end of file ***/
