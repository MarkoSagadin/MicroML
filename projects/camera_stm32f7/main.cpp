#include <string.h>
#include "sys_init.h"
#include "utility.h"
#include "flir.h"

enum state_e
{
    INIT,
    OUT_OF_SYNC,
    READING_FRAME,
    DONE
};

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



    //gpio_toggle(GPIOB, GPIO14);
    //spi_send(SPI1, counter);
    //rx_value = spi_read(SPI1);

    state_e state = INIT;
    uint16_t packet[82];
    uint16_t frame[60][80];
    uint8_t frame_row = 0;

    set_flir_agc(1);
    set_flir_telemetry(1);

    //enable_flir_cs();
    //disable_flir_cs();
    //delay(185);
    //enable_flir_cs();

    //for(int i =0; i<300; i++)
    //{
    //spi_read16(packet, 82);
    //}
    //disable_flir_cs();
    
    //printf("DONE!\n");


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
                spi_read16(packet, 82);

                if ((packet[0] & 0x0F00) == 0x0f00)
                {
                    //printf("Discard packet detected\n");
                    //Do nothing for now, you can add later some kind of timeout
                } 
                else
                {
                    //Not a discard packet
                    printf("Not a discard packet\n");
                    if ((packet[0] & 0x00FF) == 0x0)
                    {
                        //Start detected, change state so we can start moving this into frame array
                        printf("DONE!\n");
                        while(1)
                        {

                        }
                        state = READING_FRAME;
                    }
                }
                break;

            case READING_FRAME:
                // We should read ID field of each packet to be sure that it is the packet that we want
                printf("State: reading frame\n");
                if((packet[0] & 0x00FF) == frame_row)
                {
                    printf("State: reading frame, id matches frame row\n");
                    memcpy(frame[frame_row], packet + 2, 80);
                    frame_row++;

                    if (frame_row == 60)
                    {
                        //We got full frame, go to DONE
                        disable_flir_cs();
                        state = DONE;
                    }
                    else
                    {
                        //Not yet done, keep getting spi packets
                        spi_read16(packet, 2);
                    }
                }
                else
                {
                    //Error getting correct packet ID, this will have to be handeled somehow later
                    printf("ID error");
                    disable_flir_cs();
                    while(1)
                    {}
                }

                break;

            case DONE:
                printf("We are done\n");
                printf("Show some data from frame\n");

                printf("%04X", frame[0][0]);
                printf("%04X", frame[0][1]);
                printf("%04X", frame[0][2]);
                printf("%04X", frame[0][3]);


                while (1)
                {
                    gpio_toggle(GPIOB, GPIO14);
                    delay(1000);
                }

                break;
        }
    }

    return 0;
}

