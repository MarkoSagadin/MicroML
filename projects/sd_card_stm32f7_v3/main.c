#include <stdint.h>
#include <string.h>
#include "sys_init.h"
#include "utility.h"
#include "printf.h"
#include "PetitFatFS/pff.h"
#include "PetitFatFS/diskio.h"

// Global variables for writing to SD card
FATFS file_system;
char write_buffer[300] = "";
UINT byte_counter = 0;

int main(void)
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();
    i2c_setup();
    //spi_setup();

    printf("System setup done!\n");

	DSTATUS status;
	FRESULT result;
	bool flag = false; 
	
	/* Initialize physical drive */
	do {
		status = disk_initialize();
		if (status != 0) {
            printf("disk_initialize() failed\n");
			flag = false;
		} else {
            printf("disk_initialize() succeded\n");
			
			flag = true;
			
		}
		/* The application will continue to try and initialize the card.
		 * If the LED is on, try taking out the SD card and putting
		 * it back in again.  After an operation has been interrupted this is
		 * sometimes necessary.
		 */
        delay(1000);
	} while (flag == false);
	
	/* Mount volume */
	result = pf_mount(&file_system);
	
	
	if (result == FR_OK)
	{
		printf("MOUNT OK\n");	
	}
	else
	{
		printf("MOUNT NOT OK\n");	
		printf("FR is: %d\n", result);	
	}

	/* Open file */
	result = pf_open("test.txt");

	if (result == FR_OK)
	{
		printf("Opening OK\n");	
		result = pf_write("Hello there\n", 300, &byte_counter);
	    if (result == FR_OK)
		printf("Writing OK\n");	
        result = pf_write(0, 0, &byte_counter);
	    if (result == FR_OK)
		printf("Writing OK\n");	
    }
	else
	{
		printf("Opening NOT OK");	
	}

    while(1)
    {
        printf("System setup done!\n");
        delay(1000);
    }

    return 0;
}
