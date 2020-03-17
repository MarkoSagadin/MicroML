#ifndef utility_H
#define utility_H

#include <libopencm3/stm32/i2c.h>
#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool i2c_write(uint8_t addr, uint8_t data);
bool i2c_write_array(uint8_t addr, uint8_t * data, uint8_t num_bytes);
bool i2c_write16(uint8_t addr, uint16_t data);
bool i2c_write16_array(uint8_t addr, uint16_t * data, uint8_t num_words);

#ifdef __cplusplus
}
#endif
#endif /* utility_H */
/*** end of file ***/
