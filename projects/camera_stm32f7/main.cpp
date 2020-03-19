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

    printf("System setup done!\n");

    LEP_SYS_SHUTTER_POSITION position;

    display_flir_serial();

    get_flir_shutter_position();

    delay(1000);

    position = LEP_SYS_SHUTTER_POSITION_CLOSED;
    set_flir_shutter_position(position);

    delay(1000);

    get_flir_shutter_position();

    delay(1000);

    position = LEP_SYS_SHUTTER_POSITION_OPEN;
    set_flir_shutter_position(position);

    delay(1000);

    get_flir_shutter_position();

    while (1) 
    {
        gpio_set(GPIOB, GPIO14);
        delay(1000);
        gpio_clear(GPIOB, GPIO14);
        delay(1000);
    }

    return 0;
}

