#include <stdint.h>
#include <string.h>
#include "sys_init.h"
#include "utility.h"
#include "uart_ctrl.h"
#include "simple_shell.h"

int main(void)
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();

	/* At this point our console is ready to go so we can create our
	 * simple application to run on it.
	 */
    simple_shell();

    while (1)
    {
    }
}

