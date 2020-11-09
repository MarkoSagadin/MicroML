#include <libopencm3/stm32/gpio.h>
#include "sys_init.h"
#include "utility.h"
#include "printf.h"
#include "main_functions.h"

int main() 
{
    system_setup();
    
    gpio_set(GPIOB, GPIO0);
    gpio_set(GPIOB, GPIO7);
    gpio_set(GPIOB, GPIO14);
    delay(100);
    gpio_clear(GPIOB, GPIO0);
    gpio_clear(GPIOB, GPIO7);
    gpio_clear(GPIOB, GPIO14);
    delay(100);

    setup();

    printf("Setup done!");

    while(1)
    {
        loop(); 
    }
}
