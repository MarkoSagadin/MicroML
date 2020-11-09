#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include <string.h>
#include "system_setup/sys_init.h"
#include "system_setup/utility.h"
#include "system_setup/printf.h"
#include "simple_shell/simple_shell.h"
#include "inference/inference.h"

int main(void)
{
    system_setup();
    inference_setup();

    printf("Setup done\n");

    //run_inference();
    //setup();
    printf("Setup done\n");
    //loop();
	/* At this point our console is ready to go so we can create our
	 * simple application to run on it.
	 */
    simple_shell();

    printf("Inference done\n");
    while (1)
    {
        gpio_set(GPIOB, GPIO0);
        gpio_set(GPIOB, GPIO7);
        gpio_set(GPIOB, GPIO14);
        delay(1000);
        gpio_clear(GPIOB, GPIO0);
        gpio_clear(GPIOB, GPIO7);
        gpio_clear(GPIOB, GPIO14);
        delay(1000);
    }
}

