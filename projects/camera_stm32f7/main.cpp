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


    //while (1) 
    //{
        //gpio_set(GPIOB, GPIO14);
        //delay(1000);
        //gpio_clear(GPIOB, GPIO14);
        //delay(1000);
    //}

    uint16_t frame[60][82];

    set_flir_agc(1);
    set_flir_telemetry(1);

    //enable_flir_cs();
    //disable_flir_cs();
    //delay(185);
    //enable_flir_cs();

    //for(int i =0; i<300; i++)
    //{
        //spi_read16(frame[0], 82);
        //delay(1);
    //}
    //disable_flir_cs();

    //printf("DONE!\n");
    while(1)
    {
        if(get_picture(frame))
        {
            printf("We are done\n");
            printf("Show some data from frame\n");

            printf("%04X", frame[0][0]);
            printf("%04X", frame[0][1]);
            printf("%04X", frame[0][2]);
            printf("%04X\n", frame[0][3]);
            delay(1000);
        }
    }


    return 0;
}

