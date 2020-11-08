#include "uart_ctrl.h"
#include "simple_shell.h"
#include "printf.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#ifdef MINICOM_SHELL
#define CONSOLE_UART	USART3
#else
#define CONSOLE_UART	USART2
#endif

/*
 * put_char(char c)
 *
 * Send the character 'c' to the USART, wait for the USART
 * transmit buffer to be empty first.
 */
static void put_char(char c)
{
    usart_send_blocking(CONSOLE_UART, c); /* USART6: Send byte. */
}

/*
 * char = get_char(int wait)
 *
 * Check the console for a character. If the wait flag is
 * non-zero. Continue checking until a character is received
 * otherwise return 0 if called and no character was available.
 */
static char get_char(int wait)
{
	uint32_t	reg;
	do {
		reg = USART_ISR(CONSOLE_UART);
	} while ((wait != 0) && ((reg & USART_ISR_RXNE) == 0));
	return (reg & USART_ISR_RXNE) ? usart_recv_blocking(CONSOLE_UART) : '\000';
}

/*
 * void put_line(char *s)
 *
 * Send a string to the console, one character at a time, return
 * after the last character, as indicated by a NUL character, is
 * reached.
 */
void put_line(char *fmt, ...)
{
    char buf[512];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    uint32_t i = 0;
	while (buf[i] != '\000') 
    {
		put_char(buf[i]);
		i++;
	}
#ifndef MINICOM_SHELL
	put_char('\n');
#endif
}

/*
 * int get_line(char *s, int len)
 *
 * Wait for a string to be entered on the console, limited
 * support for editing characters (back space and delete)
 * end when a <CR> character is received.
 */
int get_line(char *s, int len)
{
    char *t = s;
    char c;

    *t = '\000';
    /* read until a <CR> is received */
#ifdef MINICOM_SHELL
    while ((c = get_char(1)) != '\r') {
        if ((c == '\010') || (c == '\127')) {
            if (t > s) {
                /* send ^H ^H to erase previous character */
                put_line("\010 \010");
                t--;
            }
        } else {
            *t = c;
            put_char(c);
            if ((t - s) < len) {
                t++;
            }
        }
        *t = '\000';
    }
    return t - s;
#else
    while ((c = get_char(1)) != '\n') {
        *t = c;
        put_char(c);
        if ((t - s) < len) {
            t++;
        }
        *t = '\000';
    }

    return t - s;
#endif
        /* update end of string with NUL */
}
