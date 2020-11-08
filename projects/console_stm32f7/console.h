#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libopencm3/stm32/usart.h>

#define CONSOLE_UART	USART3

void console_putc(char c);
char console_getc(int wait);
void console_puts(char *s);
int console_gets(char *s, int len);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */
/*** end of file ***/

