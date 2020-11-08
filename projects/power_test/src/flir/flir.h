#ifndef FLIR_H
#define FLIR_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "flir_defines.h"

#define FLIR_BUSY_TIMEOUT (5000)

typedef enum 
{
    INIT,
    OUT_OF_SYNC,
    READING_FRAME,
    DONE
}state_e;


// Debug options
// FLIR_DEBUG macro is used for debugging purposes, tracing path of functions
// and other problems. It is normally undefined
// FLIR_PRINT is used to display some data about FLIR, like serial number,
// current settings, etc. It is normally defined
#define FLIR_DEBUG
#define FLIR_PRINT

//General settings, set and get functions
void display_flir_serial();

LEP_SYS_SHUTTER_POSITION get_flir_shutter_position();
void set_flir_shutter_position(LEP_SYS_SHUTTER_POSITION position);

void set_flir_agc(bool enable);
bool get_flir_agc();

void set_flir_telemetry(bool enable);
bool get_flir_telemetry();

//Frame commands
bool get_picture(uint16_t frame[60][82]);


// Low level commands
LEP_RESULT get_last_flir_result();

void enable_flir_cs();
void disable_flir_cs();


#ifdef __cplusplus
}
#endif

#endif
/*** end of file ***/
