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

    uint16_t pico = 0;
    uint16_t pico1 = 0;
    uint16_t pico2 = 0;
    uint16_t pico3 = 0;
    uint16_t pico4 = 0;
    uint16_t pico5 = 0;

    enable_flir_cs();
    disable_flir_cs();
    delay(185);
    enable_flir_cs();

    spi_set_receive_only_mode(SPI1);
    spi_enable(SPI1);

    pico = spi_read(SPI1);

//1, Interrupt the receive flow by disabling SPI (SPE=0) in the specific time window while
//the last data frame is ongoing.
    //spi_disable(SPI1);
    spi_set_full_duplex_mode(SPI1);

//2. Wait until BSY=0 (the last data frame is processed).
	while ((SPI_SR(SPI1) & SPI_SR_BSY));

//3. Read data until FRLVL[1:0] = 00 (read all the received data).
//
	while (!((SPI_SR(SPI1) & (0b11 << 9)) == SPI_SR_FRLVL_FIFO_EMPTY))
    {
        pico1 = spi_read(SPI1);
        pico2++;
    }
    disable_flir_cs();
    
    delay(1000);
    ////////////////////////////////

    enable_flir_cs();
    disable_flir_cs();
    delay(185);
    enable_flir_cs();

    spi_set_receive_only_mode(SPI1);
    //spi_enable(SPI1);

    pico3 = spi_read(SPI1);

//1, Interrupt the receive flow by disabling SPI (SPE=0) in the specific time window while
//the last data frame is ongoing.
    spi_disable(SPI1);

//2. Wait until BSY=0 (the last data frame is processed).
	while ((SPI_SR(SPI1) & SPI_SR_BSY));

//3. Read data until FRLVL[1:0] = 00 (read all the received data).
//
	while (!((SPI_SR(SPI1) & (0b11 << 9)) == SPI_SR_FRLVL_FIFO_EMPTY))
    {
        pico4 = spi_read(SPI1);
        pico5++;
    }
    disable_flir_cs();
    
    delay(1000);

    ////////////////////////////////////
    //
    printf("pico is:  %04X\n", pico);
    printf("pico1 is: %04X\n", pico1);
    printf("pico2 is: %04X\n", pico2);
    printf("pico3 is: %04X\n", pico3);
    printf("pico4 is: %04X\n", pico4);
    printf("pico5 is: %04X\n", pico5);

    while(1)
    {
    }
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
                printf("State: Out of sync\n");
                //TODO Imaš problem s spijem clock se ti iz nekega razloga ne toggla ker se v 
                //spi_read(SPI1) v 
	            //while (!(SPI_SR(spi) & SPI_SR_RXNE)) zacikla.
                //
                spi_read16(packet, 82);

                if ((packet[0] & 0x0F00) == 0x0f00)
                {
                    printf("Discard packet detected\n");
                    //Do nothing for now, you can add later some kind of timeout
                } 
                else
                {
                    //Not a discard packet
                    printf("Not a discard packet\n");
                    if ((packet[0] & 0x00FF) == 0x0)
                    {
                        //Start detected, change state so we can start moving this into frame array
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
                        spi_read16(packet, 82);
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

