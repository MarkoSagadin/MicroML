#include <string.h>
#include <libopencm3/stm32/gpio.h>
#include "utility.h"
#include "uart_ctrl.h"
#include "printf.h"
#include "simple_shell.h"

static shell_cmd parse_command(char * buf, uint16_t len);
static bool execute_command(shell_cmd cmd);
static void get_command_response(shell_cmd, char * buf, uint16_t max_len);
static bool deliver_cmd(shell_cmd cmd, char * buf, uint16_t max_len);
static bool blink_exe();
static void blink_response(char * buf, uint16_t max_len);
static void invalid_cmd(char * buf, uint16_t max_len);

/*!
 * @brief       Entry point to simple shell, which does not
 *              ever return. It calls all other modules and 
 *              functions.
 */
void simple_shell()
{
    char buf[SHELL_BUF_LEN];
    uint16_t len;
    shell_cmd cmd;
    bool status;

    while (1) {
        //put_line("$ ");
        len = get_line(buf, SHELL_BUF_LEN);
        if (len) {
            cmd = parse_command(buf, len);

            //printf("Buf is %s\n", buf);
            //printf("Command is %d\n", cmd);
            //printf("LEN is %d\n", len);

            status = execute_command(cmd);
            if (status) {
                get_command_response(cmd, buf, SHELL_BUF_LEN);
                //put_line("\n$ ");
                put_line(buf);
            }
            else {
                //put_line("\n$ ");
                put_line("\nNOT OK\n");
            }
        }
    }
}

static shell_cmd parse_command(char * buf, uint16_t len)
{
    if (0 == strncmp("BLINK", buf, len)) return BLINK;
    if (0 == strncmp("ML", buf, len))    return ML;

    return INVALID_CMD;
}


static bool execute_command(shell_cmd cmd)
{
    return deliver_cmd(cmd, NULL, 0);
}

static void get_command_response(shell_cmd cmd, char * buf, uint16_t max_len)
{
    deliver_cmd(cmd, buf, max_len);
}

static bool deliver_cmd(shell_cmd cmd, char * buf, uint16_t max_len)
{
    switch (cmd)
    {
        case BLINK:
            if (!max_len) {
                blink_exe(); 
            }
            else {
                blink_response(buf, max_len);
            }
        break;

        case INVALID_CMD: invalid_cmd(buf, max_len);
            return false;

        default:
            invalid_cmd(buf, max_len);
            return false;
        break;
    }

    return true;
}


static bool blink_exe()
{
    for (int i = 0; i < 2; i++)
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
    return true;
}

static void blink_response(char * buf, uint16_t max_len)
{
    snprintf(buf, max_len, "BLINK: OK\n");
}

static void invalid_cmd(char * buf, uint16_t max_len)
{
    snprintf(buf, max_len, "INVALID COMMAND\n");
}
