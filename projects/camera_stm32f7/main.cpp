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

    uint16_t test_frame[3][3] = {{1,2,3},{4,5,6},{7,8,9}};

    //while (1) 
    //{
        //printf("We are done\n");
        //for(uint8_t row = 0; row < 3; row++)
        //{
            //for(uint8_t col = 0; col < 3; col++)
            //{
                //printf("%i ", test_frame[row][col]);
            //}
            //printf("\n");
        //}
        
        //// change it
        //for(uint8_t row = 0; row < 3; row++)
        //{
            //for(uint8_t col = 0; col < 3; col++)
            //{
                //test_frame[row][col] == 9 ? test_frame[row][col] = 1 : test_frame[row][col] += 1;
            //}
        //}
        //gpio_set(GPIOB, GPIO14);
        //delay(500);
        //gpio_clear(GPIOB, GPIO14);
        //delay(500);
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

