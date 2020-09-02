#include "sys_init.h"
#include "utility.h"
#include <libopencm3/stm32/gpio.h>

int main() 
{
    clock_setup();
    systick_setup();
    gpio_setup();

    // Toggle the LED on and off forever
    while (1) 
    {
        gpio_clear(GPIOB, GPIO0);
        gpio_clear(GPIOB, GPIO7);
        gpio_clear(GPIOB, GPIO14);
        delay(1000);
        gpio_set(GPIOB, GPIO0);
        gpio_set(GPIOB, GPIO7);
        gpio_set(GPIOB, GPIO14);
        delay(1000);
    }

    return 0;
}
