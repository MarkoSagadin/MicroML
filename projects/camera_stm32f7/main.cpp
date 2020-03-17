#include "sys_init.h"
#include "utility.h"

#define ADDRESS  (0x2A)

#define AGC (0x01)
#define SYS (0x02)
#define VID (0x03)
#define OEM (0x08)

#define GET (0x00)
#define SET (0x01)
#define RUN (0x02)
// TODO: V tem vrstnem redu, 
// * implementiraj timeout za NACK
// * poglej na kakšen način dobivaš čas, rabiš nekaj kot je millis
// * premakni delay, systick etc v utility
// * komaj potem se lahko lotiš sprejemanja podatkov. 16/03/2020 17:04:32
//Kaj dejansko potrebujem
//
//Preprosta write funkcija, daš ji addresso, array bytov in dolžino
//bool i2c_write(uint8_t addr, uint8_t * data, uint8_t num_bytes)
// lahko imaš tudi samo in pošiljaš po en byte
//bool i2c_write(uint8_t addr, uint8_t data)
// Začne se s startom in konča s stopom
// Potrebuješ tudi error handeling za ack/nack, ne mores imeti loopov, ki ti lahko
// do neskončnosti zaciklajo program; torej rabiš timeout

void i2c_request_from(uint8_t address, uint8_t num_bytes)
{
    /* Setting transfer properties */
    i2c_set_7bit_address(I2C1, address);
    i2c_set_read_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, num_bytes);
    /* start transfer */
    i2c_send_start(I2C1);
    /* important to do it afterwards to do a proper repeated start! */
    i2c_enable_autoend(I2C1);
}

uint8_t i2c_read()
{

}

/**
 * Run a write/read transaction to a given 7bit i2c address
 * If both write & read are provided, the read will use repeated start.
 * Both write and read are optional
 * @param i2c peripheral of choice, eg I2C1
 * @param addr 7 bit i2c device address
 * @param w buffer of data to write
 * @param wn length of w
 * @param r destination buffer to read into
 * @param rn number of bytes to read (r should be at least this long)
 */
void i2c_transfer9(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn)
{
	/*  waiting for busy is unnecessary. read the RM */
	if (wn) {
		i2c_set_7bit_address(i2c, addr);
		i2c_set_write_transfer_dir(i2c);
		i2c_set_bytes_to_transfer(i2c, wn);
		if (rn) {
			i2c_disable_autoend(i2c);
		} else {
			i2c_enable_autoend(i2c);
		}
		i2c_send_start(i2c);

		while (wn--) {
			bool wait = true;
			while (wait) {
				if (i2c_transmit_int_status(i2c)) {
					wait = false;
				}
				while (i2c_nack(i2c)); /* FIXME Some error */
			}
			i2c_send_data(i2c, *w++);
		}
		/* not entirely sure this is really necessary.
		 * RM implies it will stall until it can write out the later bits
		 */
		if (rn) {
			while (!i2c_transfer_complete(i2c));
		}
	}

	if (rn) {
		/* Setting transfer properties */
		i2c_set_7bit_address(i2c, addr);
		i2c_set_read_transfer_dir(i2c);
		i2c_set_bytes_to_transfer(i2c, rn);
		/* start transfer */
		i2c_send_start(i2c);
		/* important to do it afterwards to do a proper repeated start! */
		i2c_enable_autoend(i2c);

		for (size_t i = 0; i < rn; i++) {
			while (i2c_received_data(i2c) == 0);
			r[i] = i2c_get_data(i2c);
		}
	}
}

int main() 
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();
    i2c_setup();


    printf("FIRST OUTPUT\n");

    // We are getting WHO_AM_I register from LIS2DW12
    //uint8_t addr = 0x19;
    //uint8_t cmd = 0x0F;
    uint8_t data;

    uint16_t data_tran[4] = {0x1234, 0x5678, 0xABCD, 0x6969};
    while (1) 
    {
        //i2c_transfer7(I2C1, ADDRESS, data_tran, 2, &data, 0);
        //printf("Received this = %d\n", data);
        
        //i2c_write(ADDRESS, data_tran, 2);
        //i2c_write16_array(ADDRESS, data_tran, 4);
        //gpio_set(GPIOB, GPIO14);
        gpio_set(GPIOA, GPIO0);
        delay_us(500);
        gpio_clear(GPIOA, GPIO0);
        delay_us(500);
    }

    return 0;
}

