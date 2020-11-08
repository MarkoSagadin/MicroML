#include <libopencm3/stm32/gpio.h>
#include <stdarg.h>
#include <stdio.h>
#include "system_setup/utility.h"
#include "system_setup/printf.h"
#include "flir.h"

static uint8_t last_flir_error = LEP_OK;

// Low level commands
static bool wait_busy_bit(uint16_t timeout);
static bool get_flir_command(uint16_t cmd_code, 
                             uint16_t * data_words, 
                             uint8_t num_words);
static bool get_flir_command32(uint16_t cmd_code, 
                               uint32_t * data_long_word);
static bool set_flir_command32(uint16_t cmd_code, 
                               uint32_t data_long_word);
static uint16_t command_code(uint16_t cmd_id, uint16_t cmd_type);

//I2C commands, i2c peripheral should be initialized before using flir.h
static bool write_register(uint16_t reg_address, uint16_t value);
static bool read_register(uint16_t reg_address, uint16_t * value);
static bool write_command_register(uint16_t cmd_code, 
                                   uint16_t * data_words, 
                                   uint16_t num_words);
static bool read_data_register(uint16_t * read_words, uint8_t max_length);

// Utility functions, hardware depended
static void flir_delay(uint32_t delay);
static uint32_t flir_millis();

// Enum to string functions
static const char * shutter_position_str(LEP_SYS_SHUTTER_POSITION position);


/*!
 * @brief     Debug output wrapper
 *
 * @param[in] string  
 * @param[in] variable number of arguments  
 */
void flir_debug(char* fmt, ...)
{
#ifdef FLIR_DEBUG
    va_list args;
    va_start(args, fmt);
    printf("FLIR_DEBUG: ");
    vprintf(fmt, args);
    va_end(args);
#endif 
}

void flir_print(char* fmt, ...)
{
#ifdef FLIR_PRINT
    va_list args;
    va_start(args, fmt);
    printf("FLIR_PRINT: ");
    vprintf(fmt, args);
    va_end(args);
#endif 
}

/*!
 * @brief           Display FLIR serial number
 *
 * @note            Function prints fail, if something went wrong
 */
void display_flir_serial()
{
    uint16_t serial_num[4] = {0,0,0,0};

    if(get_flir_command(command_code(LEP_CID_SYS_FLIR_SERIAL_NUMBER, 
                                     LEP_I2C_COMMAND_TYPE_GET), 
                                     serial_num, 4))
    {
        flir_print("SYS Flir Serial Number: %04X%04X%04X%04X\n", 
                                              serial_num[3],
                                              serial_num[2],
                                              serial_num[1],
                                              serial_num[0]);
    }
    else
    {
        flir_print("SYS Flir Serial Number: Fail\n");
    }
}

/*!
 * @brief                              Function gets current position of shutter
 *
 * @return LEP_SYS_SHUTTER_POSITION    enum defined in flir_defines.h file
 *
 * @note            Function prints fail, if something went wrong
 */
LEP_SYS_SHUTTER_POSITION get_flir_shutter_position()
{
    uint32_t position = 0;

    if(get_flir_command32(command_code(LEP_CID_SYS_SHUTTER_POSITION, 
                                       LEP_I2C_COMMAND_TYPE_GET), 
                                       &position))
    {
        
        flir_print("Shutter position: %s\n", 
                shutter_position_str((LEP_SYS_SHUTTER_POSITION) position));
    }
    else
    {
        flir_print("Shutter position: function failed!\n");
    }
    
    return (LEP_SYS_SHUTTER_POSITION) position;
}

/*!
 * @brief               Function sets position of shutter
 *
 * @param[in] position  
 */
void set_flir_shutter_position(LEP_SYS_SHUTTER_POSITION position)
{ 
    if(!set_flir_command32(command_code(LEP_CID_SYS_SHUTTER_POSITION, 
                    LEP_I2C_COMMAND_TYPE_SET), (uint32_t) position))
    {
        flir_print("Set shutter position : function failed!\n");
    }
}

/*!
 * @brief               Enable or disable AGC processing
 *
 * @param[in] position  If true AGC will be enabled
 *
 * @note                Function prints fail, if something went wrong
 */
void set_flir_agc(bool enable)
{
    if(!set_flir_command32(command_code(LEP_CID_AGC_ENABLE_STATE, 
                                        LEP_I2C_COMMAND_TYPE_SET), 
                                        (uint32_t) enable))
    {
        flir_print("AGC mode: function failed!\n");
    }
}

/*!
 * @brief   Get current state of AGC mode
 *
 * @return  True if AGC mode is enabled, otherwise false
 *
 * @note    Function prints fail, if something went wrong
 */
bool get_flir_agc()
{
    uint32_t agc_state = 0;
    if(get_flir_command32(command_code(LEP_CID_AGC_ENABLE_STATE, 
                                       LEP_I2C_COMMAND_TYPE_GET), 
                                       &agc_state))
    {
        flir_print("AGC mode: %s\n", agc_state ? "On" : "Off"); 
    }
    else
    {
        flir_print("AGC mode: function failed!\n");
    }
    return (bool) agc_state;
}


/*!
 * @brief               Enable or disable AGC processing
 *
 * @param[in] position  If true AGC will be enabled
 *
 * @note                Function prints fail, if something went wrong
 */
void set_flir_telemetry(bool enable)
{
    if(!set_flir_command32(command_code(LEP_CID_SYS_TELEMETRY_ENABLE_STATE, 
                                        LEP_I2C_COMMAND_TYPE_SET), 
                                        (uint32_t) enable))
    {
        flir_print("Set Telemetry : function failed!\n");
    }
}

/*!
 * @brief   Get current state of AGC mode
 *
 * @return  True if AGC mode is enabled, otherwise false
 *
 * @note    Function prints fail, if something went wrong
 */
bool get_flir_telemetry()
{
    uint32_t telemetry_state = 0;
    if(get_flir_command32(command_code(LEP_CID_SYS_TELEMETRY_ENABLE_STATE, 
                                       LEP_I2C_COMMAND_TYPE_GET), 
                                       &telemetry_state))
    {
        flir_print("Telemetry: %s\n", telemetry_state ? "On" : "Off"); 
    }
    else
    {
        flir_print("Telemetry: function failed!\n");
    }
    return (bool) telemetry_state ;
}

// Frame commands
bool get_picture(uint16_t frame[60][82])
{
    state_e state = INIT;
    uint8_t frame_row = 0;

    while(1)
    {
        switch(state)
        {
            case INIT:
                enable_flir_cs();
                disable_flir_cs();
                delay(185);
                enable_flir_cs();
                state = OUT_OF_SYNC;
                break;

            case OUT_OF_SYNC:
                spi_read16(frame[frame_row], 82);

                if ((frame[frame_row][0] & 0x0F00) == 0x0f00)
                {
                    //flir_print("Discard packet detected\n");
                    //Do nothing for now, you can add later some kind of timeout
                } 
                else
                {
                    //Not a discard packet
                    if ((frame[frame_row][0] & 0x00FF) == 0x0)
                    {
                        //Start detected, change state so we can start moving this into frame array
                        frame_row++;
                        state = READING_FRAME;
                    }
                }
                break;

            case READING_FRAME:
                spi_read16(frame[frame_row], 82);

                // We should read ID field of each packet to be sure that it is the packet that we want
                if((frame[frame_row][0] & 0x00FF) == frame_row)
                {
                    //flir_print("Match");
                    frame_row++;

                    if (frame_row == 60)
                    {
                        //We got full frame, go to DONE
                        disable_flir_cs();
                        state = DONE;
                    }
                }
                else
                {
                    //Error getting correct packet ID, this will have to be handeled somehow later
                    flir_print("ID error\n");
                    flir_print("Expected frame_row: %d\n", frame_row);
                    flir_print("What we got:        %d\n", (frame[frame_row][0] & 0x00FF));
                    disable_flir_cs();
                    return false;
                    delay(10);
                    frame_row = 0;
                    state = INIT;
                }
                break;

            case DONE:
                flir_print("DONE!\n");
                return true;
                break;
        }
    }
}


/*!
 * @brief                   Sends get command to FLIR module
 *
 * @param[in] cmd_code      Command code to be send, use ones 
 *                          defined in flir_defines.h
 * @param[in] data_words    Copy by reference, result will be copied into it
 * @param[in] num_words     Num of words that will be read
 *
 * @return                  True if everything is ok, otherwise false
 *
 * @note    Procedure implemented as written in Lepton Software Interface 
 *          Description Document rev200.pdf, page 11
 */
static bool get_flir_command(uint16_t cmd_code, uint16_t * data_words, uint8_t num_words)
{
    // Read command status register
    // If not BUSY, write number of data words to read into DATA length reg.
    if(wait_busy_bit(FLIR_BUSY_TIMEOUT))
    {
        // write command ID to command reg
        // Read command status register
        if (write_register(LEP_I2C_COMMAND_REG, cmd_code)) 
        {
            // Successful wait for Flir to process this
            if(wait_busy_bit(FLIR_BUSY_TIMEOUT))
            {
                // If no timeout, read DATA from DATA regs
                if(read_data_register(data_words, num_words))
                {
                    return true; 
                }
            }
        }
    }
    // Something failed
    return false;
}

/*!
 * @brief                   Sends get command to FLIR module
 *
 * @param[in] cmd_code      Command code to be send, use ones 
 *                          defined in flir_defines.h
 * @param[in] data_words    Copy by reference, result will be copied into it
 * @param[in] num_words     Num of words that will be read
 *
 * @return                  True if everything is ok, otherwise false
 *
 * @note    Procedure implemented as written in Lepton Software Interface 
 *          Description Document rev200.pdf, page 11
 *          This seems like unnecessary duplicate of the other set function,
 *          but it will make high level use easier
 */
static bool get_flir_command32(uint16_t cmd_code, uint32_t * data_long_word)
{
    // Read command status register
    // If not BUSY, write number of data words to read into DATA length reg.
    if(wait_busy_bit(FLIR_BUSY_TIMEOUT))
    {
        // write command ID to command reg
        // Read command status register
        if (write_register(LEP_I2C_COMMAND_REG, cmd_code)) 
        {
            // Successful wait for Flir to process this
            if(wait_busy_bit(FLIR_BUSY_TIMEOUT))
            {
                // If no timeout, read DATA from DATA regs
                if(read_data_register((uint16_t * ) data_long_word, 2))
                {
                    return true; 
                }
            }
        }
    }
    // Something failed
    return false;
}


/*!
 * @brief                       Sends set command to FLIR module
 *
 * @param[in] cmd_code          Command code to be send, use the ones 
 *                              defined in flir_defines.h
 * @param[in] data_long_word    Copy by value
 * @param[in] num_words         Num of words that will be read
 *
 * @return                      True if everything is ok, otherwise false
 *
 * @note    Procedure implemented as written in Lepton Software Interface 
 *          Description Document rev200.pdf, page 12
 *          This seems like unnecessary duplicate of the other set function,
 *          but it will make high level use easier
 */
static bool set_flir_command32(uint16_t cmd_code, uint32_t data_long_word)
{
    // Read command status register
    // If not BUSY, write number of data words to read into DATA length reg.
    if (wait_busy_bit(FLIR_BUSY_TIMEOUT))
    {
        // write command ID to command reg
        // Read command status register
        if (write_command_register(cmd_code, (uint16_t *) &data_long_word, 2))
        {
            // Successful wait for Flir to process this
            if (wait_busy_bit(FLIR_BUSY_TIMEOUT))
            {
                return true; 
            }
        }
    }

    // Something failed
    return false;
}

/*!
 * @brief                   Function will convert command ID and type 
 *                          into one cmd_code, which will be fed into 
 *                          get_flir_command() or set_flir_command32()
 *
 * @param[in] cmd_id        Like LEP_CID_VID_POLARITY_SELECT
 * @param[in] cmd_type      Like LEP_I2C_COMMAND_TYPE_GET
 *
 * @return cmd_code
 *
 * @note 
 */
static uint16_t command_code(uint16_t cmd_id, uint16_t cmd_type) 
{
    return (cmd_id & LEP_I2C_COMMAND_MODULE_ID_BIT_MASK) | 
           (cmd_id & LEP_I2C_COMMAND_ID_BIT_MASK) | 
           (cmd_type & LEP_I2C_COMMAND_TYPE_BIT_MASK);
}

/*!
 * @brief                   Blocking function, will wait until FLIR camera is 
 *                          ready to receive commands
 *
 * @param[in] timeout       How long will function pool for BUSY bit 
 *                          in milliseconds
 *
 * @return                  True if BUSY bit was cleared or false 
 *                          if timeout was reached
 */
static bool wait_busy_bit(uint16_t timeout)
{
    uint16_t status;

    if (!read_register(LEP_I2C_STATUS_REG, &status))
    {
        // Something went wrong
        return false;
    }

    if (!(status & LEP_I2C_STATUS_BUSY_BIT_MASK))
    {
        // Busy bit was cleared
        last_flir_error = (uint8_t)
                          ((status & LEP_I2C_STATUS_ERROR_CODE_BIT_MASK) >>  
                                     LEP_I2C_STATUS_ERROR_CODE_BIT_SHIFT);
        return true;
    }

    // Get current time and add to it timeout value
    uint32_t end_time = flir_millis() + (uint32_t) timeout;

    // Loop as long busy bit is set and timeout isn't reached
    while ((status & LEP_I2C_STATUS_BUSY_BIT_MASK) && flir_millis() < end_time)
    {
        flir_delay(1);

        if(!read_register(LEP_I2C_STATUS_REG, &status))
        {
            // Something went wrong
            return false;
        }
    }

    if (!(status & LEP_I2C_STATUS_BUSY_BIT_MASK))
    {
        // Busy bit was cleared
        last_flir_error = (uint8_t)
                          ((status & LEP_I2C_STATUS_ERROR_CODE_BIT_MASK) >> 
                                     LEP_I2C_STATUS_ERROR_CODE_BIT_SHIFT);
        return true;
    }
    else
    {
        // Busy bit was still set after timeout
        return false;
    }
}

/*!
 * @brief                   Returns static variable that will show last 
 *                          error code given from FLIR
 *
 * @return error code
 *
 * @note                    Check LEP_RESULT enum for possible values
 */
LEP_RESULT get_last_flir_result()
{
    return (LEP_RESULT) last_flir_error;
}

//I2C commands, i2c peripheral should be initialized before using flir.h

/*!
 * @brief                   Writes two bytes into FLIR register
 *
 * @param[in] reg_address   Register address that will be written
 * @param[in] value         Value that will be written in register  
 *
 * @return                  True if everything ok, otherwise false
 */
static bool write_register(uint16_t reg_address, uint16_t value)
{
    uint16_t temp_buf[2] = {reg_address, value};
    if (!i2c_write16_array(LEP_I2C_DEVICE_ADDRESS, temp_buf, 2))
    {
        return false;
    }
    return true;
}

/*!
 * @brief                   Read FLIR register
 *
 * @param[in] reg_address  
 * @param[in] value         Passed by reference, will hold read value after
 *                          function call
 *
 * @return                  True if everything ok, otherwise false
 */
static bool read_register(uint16_t reg_address, uint16_t * value)
{
    if (!i2c_write16(LEP_I2C_DEVICE_ADDRESS, reg_address))
    {
        return false;
    }

    return i2c_read16(LEP_I2C_DEVICE_ADDRESS, value);
}

/*!
 * @brief                   Functions writes into command register of 
 *                          FLIR camera
 *
 * @param[in] cmd_code      Command code that will be written
 * @param[in] data_words      
 * @param[in] num_words
 *
 * @return                  True if everything ok, otherwise false
 */
static bool write_command_register(uint16_t cmd_code, 
                                   uint16_t * data_words, 
                                   uint16_t num_words)
{
    uint16_t data[2];
    if (data_words && num_words)
    {
        data[0] = LEP_I2C_DATA_LENGTH_REG;
        data[1] = num_words;

        if(!i2c_write16_array(LEP_I2C_DEVICE_ADDRESS, data, 2))
        {
            // Something went wrong
            return false;
        }

        uint16_t reg_address = num_words <= 16 ? LEP_I2C_DATA_0_REG : 
                                                 LEP_I2C_DATA_BUFFER;


        // If you are wondering why this has to be sent out in the weird way
        // read comment next to function definition
        if (!i2c_write_two_16_array(LEP_I2C_DEVICE_ADDRESS, 
                                    &reg_address, 1, 
                                    data_words, num_words))
        {
            return false;
        }
    }

    data[0] = LEP_I2C_COMMAND_REG;
    data[1] = cmd_code;

    if(!i2c_write16_array(LEP_I2C_DEVICE_ADDRESS, data, 2))
    {
        // Something went wrong
        return false;
    }

    return true;
}

/*!
 * @brief                   Functions reads DATA register of FLIR camera
 *
 * @param[in] read_words  
 * @param[in] max_length    That can be read in words  
 *
 * @return                  True, if everything ok, otherwise false
 * @note                    When transmitting DATA that are larger than a 
 *                          single word (16-bits), the larger DATA is divided 
 *                          into multiple 16-bit words; each word is then placed 
 *                          into multiple DATA registers with the least 
 *                          significant word in the lower DATA register. 
 *                          Thus for a 32-bit transfer, a Host would place the 
 *                          lower 16-bits into DATA 0 (Least Significant Word 
 *                          first) and the upper 16-bits into DATA 1.
 */
static bool read_data_register(uint16_t * read_words, uint8_t max_length)
{
    // Point to data length reg
    if(!i2c_write16(LEP_I2C_DEVICE_ADDRESS, LEP_I2C_DATA_LENGTH_REG))
    {
        // Something went wrong
        return false;
    }

    // Get data length
    // Now something weird is happening here. Here we are going to read DATA 
    // length registers, which should tell us how many WORDS of data are we 
    // expecting (according to datasheet) but is actually returning number 
    // of BYTES that we will have to read. For example when getting Customer
    // Serial number we are expecting DATA length to throw us number 16 as 
    // per datasheet, BUT we get 32. So it seems that we will be using number 
    // of bytes instead.  
    uint16_t num_bytes = 0;
    if (!i2c_read16(LEP_I2C_DEVICE_ADDRESS, &num_bytes))
    {
        // Something went wrong
        return false;
    }
    
    // Error handling
    if (0 == num_bytes)
    {
        return false;
    }
    
    // Error handling
    if ((max_length * 2) != num_bytes)
    {
        return false;
    }


    if (!i2c_read16_array(LEP_I2C_DEVICE_ADDRESS, read_words, max_length))
    {
        return false;
    }

    return true;
}

// Utility functions, hardware depended

/*!
 * @brief                   Function will delay processor for specified time
 *
 * @param[in] delay         In milliseconds  
 *
 * @note                    Hardware depended implementation
 */
static void flir_delay(uint32_t delay_in_ms)
{
    delay(delay_in_ms);
}

/*!
 * @brief               Get value how long is processor running in milliseconds
 *
 * @return time         In milliseconds
 *
 * @note                Hardware depended implementation
 */
static uint32_t flir_millis()
{
    return millis();
}


/*!
 * @brief               Used to decode enum for shutter position
 *
 * @param[in] position  of enum type  
 *
 * @return              String describing position of shutter 
 *
 * @note            Use only for printing
 */
static const char * shutter_position_str(LEP_SYS_SHUTTER_POSITION position)
{
    switch(position)
    {
        case LEP_SYS_SHUTTER_POSITION_UNKNOWN:  return "unknown";
        case LEP_SYS_SHUTTER_POSITION_IDLE :    return "idle";
        case LEP_SYS_SHUTTER_POSITION_OPEN:     return "open";
        case LEP_SYS_SHUTTER_POSITION_CLOSED:   return "closed";
        case LEP_SYS_SHUTTER_POSITION_BRAKE_ON: return "brake on";
        default:                                return "unknown";
    }
}


/*!
 * @brief               Enables chip select line for FLIR, pulls it down
 */
void enable_flir_cs()
{
    gpio_clear(GPIOB, GPIO4);
}

/*!
 * @brief               Disables chip select line for FLIR, pulls it up
 */
void disable_flir_cs()
{
    gpio_set(GPIOB, GPIO4);
}
