#include <errno.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
#include "printf.h"

extern "C" {
    void sys_tick_handler(void);
}

void _putchar(char character)
{
    usart_send_blocking(USART3, character); /* USART6: Send byte. */
}

static void usart_setup(void)
{
	/* Setup USART2 parameters. */
	usart_set_baudrate(USART3, 115200);
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_mode(USART3, USART_MODE_TX);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART3);
}

static void clock_setup() {
    // First, let's ensure that our clock is running off the high-speed internal
    // oscillator (HSI) at 48MHz.
    //rcc_clock_setup_in_hsi_out_48mhz();

    // Since our LED is on GPIO bank A, we need to enable
    // the peripheral clock to this GPIO bank in order to use it.
    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_GPIOB);

    // In order to use our UART, we must enable the clock to it as well.
    rcc_periph_clock_enable(RCC_USART3);
}
static void systick_setup() {
    // Set the systick clock source to our main clock
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    // Clear the Current Value Register so that we start at 0
    STK_CVR = 0;
    // In order to trigger an interrupt every millisecond, we can set the reload
    // value to be the speed of the processor / 1000 -1
    systick_set_reload(rcc_ahb_frequency / 1000 - 1);
    // Enable interrupts from the system tick clock
    systick_interrupt_enable();
    // Enable the system tick counter
    systick_counter_enable();
}

// Storage for our monotonic system clock.
// Note that it needs to be volatile since we're modifying it from an interrupt.
static volatile uint64_t _millis = 0;

uint64_t millis() {
    return _millis;
}

// This is our interrupt handler for the systick reload interrupt.
// The full list of interrupt services routines that can be implemented is
// listed in libopencm3/include/libopencm3/stm32/f0/nvic.h
void sys_tick_handler(void) {
    // Increment our monotonic clock
    _millis++;
}
/**
 * Delay for a real number of milliseconds
 */

void delay(uint64_t duration) {
    const uint64_t until = millis() + duration;
    while (millis() < until);
}

static void gpio_setup() {
    // Our test LED is connected to Port A pin 11, so let's set it as output
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);

	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);

	/* Setup USART2 TX pin as alternate function. */
	gpio_set_af(GPIOD, GPIO_AF7, GPIO8);
}

int main() 
{
    clock_setup();
    systick_setup();
    gpio_setup();
	usart_setup();
    printf("FIRST OUTPUT\n");

    // Toggle the LED on and off forever
    while (1) 
    {
        printf("HELLO WORLD\n");
        printf("\n");
		//usart_send_blocking(USART3, 'A'); /* USART6: Send byte. */
        gpio_set(GPIOB, GPIO7);
        delay(500);
        gpio_clear(GPIOB, GPIO7);
        delay(500);
    }

    return 0;
}

