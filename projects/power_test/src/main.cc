#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include <string.h>
#include "system_setup/sys_init.h"
#include "system_setup/utility.h"
#include "system_setup/printf.h"
#include "simple_shell/simple_shell.h"
//#include "inference/inference.h"
#include "flir/flir.h"


uint16_t imagex[60][82];
int main(void)
{
    system_setup();
    //inference_setup();
    flir_setup();

    printf("Setup done\n");

	/* At this point our console is ready to go so we can create our
	 * simple application to run on it.
	 */
    int i = 0;
    while (1)
    {
      if (get_flir_image(imagex)){
          printf("GOOD, %d\n", i++);

          for(uint8_t row = 0; row < 20; row++)
          {
              for(uint8_t col = 2; col < 20; col++)
              {
                  printf("%d ", (uint8_t)imagex[row][col]);
              }
              printf("\n");
          }
          delay(1000);
      }
      else
      {
          printf("BAD\n");
      }
    }

    //simple_shell();

    while (1)
    {
        gpio_set(GPIOB, GPIO0);
        gpio_set(GPIOB, GPIO7);
        gpio_set(GPIOB, GPIO14);
        delay(100);
        gpio_clear(GPIOB, GPIO0);
        gpio_clear(GPIOB, GPIO7);
        gpio_clear(GPIOB, GPIO14);
        delay(100);
    }
}

