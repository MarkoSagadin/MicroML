#include "utility.h"

bool i2c_write(uint8_t addr, uint8_t data)
{
    i2c_set_7bit_address(I2C1, addr);
    i2c_set_write_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, 1);
    i2c_enable_autoend(I2C1);
    i2c_send_start(I2C1);

    // We wait until we send out all data in TXDR and we get ACK back.
    // This could also be dangerous, as we could be waiting forever
    bool wait = true;
    while (wait)
    {
        if (i2c_transmit_int_status(I2C1)) 
        {
            wait = false;
        }
        while (i2c_nack(I2C1));
    }
    i2c_send_data(I2C1, data);
    return true;
}

bool i2c_write_array(uint8_t addr, uint8_t * data, uint8_t num_bytes)
{
    i2c_set_7bit_address(I2C1, addr);
    i2c_set_write_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, num_bytes);
    i2c_enable_autoend(I2C1);
    i2c_send_start(I2C1);

    // We wait until we send out all data in TXDR and we get ACK back.
    // This could also be dangerous, as we could be waiting forever
    while (num_bytes--) 
    {
        bool wait = true;
        while (wait) 
        {
            if (i2c_transmit_int_status(I2C1)) 
            {
                wait = false;
            }
            while (i2c_nack(I2C1));
        }
        i2c_send_data(I2C1, *data++);
    }
    return true;
}

bool i2c_write16(uint8_t addr, uint16_t data)
{
    uint8_t separated_bytes[2] = {(uint8_t) (data >> 8) & 0xFF, 
                                 (uint8_t) data & 0xFF};

    i2c_set_7bit_address(I2C1, addr);
    i2c_set_write_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, 2);
    i2c_enable_autoend(I2C1);
    i2c_send_start(I2C1);

    // We wait until we send out all data in TXDR and we get ACK back.
    // This could also be dangerous, as we could be waiting forever
    for(int i = 0; i < 2; i++)
    {
        bool wait = true;
        while (wait) 
        {
            if (i2c_transmit_int_status(I2C1)) 
            {
                wait = false;
            }
            while (i2c_nack(I2C1));
        }
        i2c_send_data(I2C1, separated_bytes[i]);
    }
    return true;
}

bool i2c_write16_array(uint8_t addr, uint16_t * data, uint8_t num_words)
{
    i2c_set_7bit_address(I2C1, addr);
    i2c_set_write_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, 2 * num_words);
    i2c_enable_autoend(I2C1);
    i2c_send_start(I2C1);

    // We wait until we send out all data in TXDR and we get ACK back.
    // This could also be dangerous, as we could be waiting forever
    bool high_byte = true;
    uint16_t num_bytes = 2 * num_words;

    while (num_bytes--) 
    {
        bool wait = true;
        while (wait) 
        {
            if (i2c_transmit_int_status(I2C1)) 
            {
                wait = false;
            }
            while (i2c_nack(I2C1));
        }
        if (high_byte)
        {
            //time to send high byte of the data element
            uint8_t byte = (uint8_t) (*data >> 8) & 0xFF;
            //send it
            i2c_send_data(I2C1, byte);
            high_byte = false;
        }
        else
        {
            //time to send low byte of the data element
            uint8_t byte = (uint8_t) (*data & 0xFF);
            i2c_send_data(I2C1, byte);
            data++;                     // Increment pointer to next element
            high_byte = true;
        }
    }
    return true;
}
