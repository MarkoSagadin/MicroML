#include "console.h"


/*
 * console_putc(char c)
 *
 * Send the character 'c' to the USART, wait for the USART
 * transmit buffer to be empty first.
 */
void console_putc(char c)
{
    usart_send_blocking(CONSOLE_UART, c); /* USART6: Send byte. */
}

/*
 * char = console_getc(int wait)
 *
 * Check the console for a character. If the wait flag is
 * non-zero. Continue checking until a character is received
 * otherwise return 0 if called and no character was available.
 */
char console_getc(int wait)
{
	uint32_t	reg;
	do {
		reg = USART_ISR(CONSOLE_UART);
	} while ((wait != 0) && ((reg & USART_ISR_RXNE) == 0));
	return (reg & USART_ISR_RXNE) ? usart_recv_blocking(CONSOLE_UART) : '\000';
}

/*
 * void console_puts(char *s)
 *
 * Send a string to the console, one character at a time, return
 * after the last character, as indicated by a NUL character, is
 * reached.
 */
void console_puts(char *s)
{
	while (*s != '\000') 
    {
		console_putc(*s);
		/* Add in a carraige return, after sending line feed */
		if (*s == '\n') 
        {
			console_putc('\r');
		}
		s++;
	}
}

/*
 * int console_gets(char *s, int len)
 *
 * Wait for a string to be entered on the console, limited
 * support for editing characters (back space and delete)
 * end when a <CR> character is received.
 */
int console_gets(char *s, int len)
{
	char *t = s;
	char c;

	*t = '\000';
	/* read until a <CR> is received */
	while ((c = console_getc(1)) != '\r') 
    {
		if ((c == '\010') || (c == '\127')) 
        {
			if (t > s) 
            {
				/* send ^H ^H to erase previous character */
				console_puts("\010 \010");
				t--;
			}
		} 
        else 
        {
			*t = c;
			console_putc(c);
			if ((t - s) < len) 
            {
				t++;
			}
		}
		/* update end of string with NUL */
		*t = '\000';
	}
	return t - s;
}
