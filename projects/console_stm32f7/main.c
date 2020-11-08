#include <stdint.h>
#include <string.h>
#include "sys_init.h"
#include "console.h"

int main(void)
{
	char buf[128];
	int	len;

    clock_setup();
    systick_setup();
    usart_setup();

    printf("System setup done!\n");

	/* At this point our console is ready to go so we can create our
	 * simple application to run on it.
	 */
	console_puts("\nConsole Demonstration Application\n");
	while (1) {
		console_puts("Enter a string: ");
		len = console_gets(buf, 128);
		if (len) {
			console_puts("\nYou entered : '");
			console_puts(buf);
			console_puts("'\n");
		} else {
			console_puts("\nNo string entered\n");
		}
	}
}
