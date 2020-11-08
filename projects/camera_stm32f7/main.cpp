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
    //while(1)
    //{
    //    set_flir_shutter_position(LEP_SYS_SHUTTER_POSITION_OPEN);
    //    delay(1000);
    //    set_flir_shutter_position(LEP_SYS_SHUTTER_POSITION_CLOSED);
    //    delay(1000);
    //} 

    delay(750);

    set_flir_agc(1);
    set_flir_telemetry(1);

    printf("FLIR setup done!\n");

    while(1)
    {
        if(get_picture(frame))
        {
            printf("We made a image\n");
            //for(uint8_t row = 0; row < 60; row++)
            //{
            //    for(uint8_t col = 2; col < 82; col++)
            //    {
            //        printf("%hhu ", (uint8_t)frame[row][col]);
            //    }
            //    printf("\n");
            //}
            delay(1000);
        }
    }

    return 0;
}

