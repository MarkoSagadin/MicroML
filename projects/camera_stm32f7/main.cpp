#include <string.h>
#include "sys_init.h"
#include "utility.h"
#include "flir.h"

int main() 
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();
    i2c_setup();
    spi_setup();

    printf("System setup done!\n");


    uint16_t frame[60][82];

    set_flir_agc(1);
    set_flir_telemetry(1);

    //enable_flir_cs();
    //disable_flir_cs();
    //delay(185);
    //enable_flir_cs();

    //for(int i = 0; i < 1000; i++)
    //{
    //spi_read16(frame[0], 82);
    ////delay(1);
    //}

    //disable_flir_cs();

   printf("DONE!\n");

    while(1)
    {
        if(get_picture(frame))
        {
            printf("We are done\n");
            for(uint8_t row = 0; row < 60; row++)
            {
                for(uint8_t col = 2; col < 82; col++)
                {
                    printf("%hhu ", (uint8_t)frame[row][col]);
                }
                printf("\n");
            }
            delay(1000);
        }
    }

    return 0;
}

