#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/dwt.h>
#include "printf.h"

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

static void clock_setup() 
{
    rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_48MHZ]);

    // Since our LED is on GPIO bank A, we need to enable
    // the peripheral clock to this GPIO bank in order to use it.
    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_GPIOB);

    // In order to use our UART, we must enable the clock to it as well.
    rcc_periph_clock_enable(RCC_USART3);
}

static void systick_setup() 
{
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

uint64_t millis() 
{
    return _millis;
}

// This is our interrupt handler for the systick reload interrupt.
// The full list of interrupt services routines that can be implemented is
// listed in libopencm3/include/libopencm3/stm32/f0/nvic.h
void sys_tick_handler(void) 
{
    // Increment our monotonic clock
    _millis++;
}
/**
 * Delay for a real number of milliseconds
 */

void delay(uint64_t duration) 
{
    const uint64_t until = millis() + duration;
    while (millis() < until);
}

static void gpio_setup() 
{
    // Our test LED is connected to Port A pin 11, so let's set it as output
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);

	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);

	/* Setup USART2 TX pin as alternate function. */
	gpio_set_af(GPIOD, GPIO_AF7, GPIO8);
}

uint32_t cycles_to_ms(uint32_t cycles)
{
    // 48 represents the clock frequency in MHz
    return cycles * (1.0f/(48*1000.0f)) ;
}

int main() 
{
    clock_setup();
    systick_setup();
    gpio_setup();
	usart_setup();
    printf("FIRST OUTPUT\n");

    // Unlock access to DWT otherwise we can not use it, might be only needed
    // on cortex-m7
    DWT_LAR |= 0xC5ACCE55;           

    if(dwt_enable_cycle_counter())
    {
        printf("DWT enabled\n");
    }
    else
    {
        printf("DWT not availible\n");
    }

    // Toggle the LED on and off forever, measure cycles
    while (1) 
    {
        printf("HELLO WORLD\n");

        gpio_set(GPIOB, GPIO7);
        volatile uint32_t start = dwt_read_cycle_counter();
        delay(500);
        volatile uint32_t end = dwt_read_cycle_counter();
        printf("Time needed: %d\n", cycles_to_ms(end-start));

        gpio_clear(GPIOB, GPIO7);
        start = dwt_read_cycle_counter();
        delay(500);
        end = dwt_read_cycle_counter();
        printf("Time needed: %d\n", cycles_to_ms(end-start));
    }

    return 0;
}

