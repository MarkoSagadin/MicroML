#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/systick.h>
#include "printf.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
 * This is a simple setup for I2C periphial, we set 48MHz as our SYSCLK, I2C1 is running from HSI (16MHz). 
 * I2C1 pins are PB8 and PB9
 * We simply sent reg command 0x0F to addr 0x19, we are expecting replay 0x68
 * We also set MCO1 and MC02 pins to show us our HSI and SYSCLK frequencies, both have div of 4.
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void _putchar(char character)
{
    usart_send_blocking(USART3, character); /* USART3: Send byte. */
}

extern "C" {
    void sys_tick_handler(void);
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

static void clock_setup()
{
    // First, let's ensure that our clock is running off the high-speed internal
    // oscillator (HSI) at 48MHz.
    rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_48MHZ]);

    // Turn on MCO1 and MCO2 pins which show you internal frequencies
    // Both will have prescaler division of 4

    // MCO1 setup on pin PA8, showing HSI
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO8);
    RCC_CFGR |= (RCC_CFGR_MCOPRE_DIV_4 << RCC_CFGR_MCO1PRE_SHIFT);

    // MCO2 setup on pin PC9, showing SYSCLK
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
	gpio_set_af(GPIOC, GPIO_AF0, GPIO9);
    RCC_CFGR |= (RCC_CFGR_MCOPRE_DIV_4 << RCC_CFGR_MCO2PRE_SHIFT);
}

static void i2c_setup(void)
{
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_GPIOB);
	delay(1); //Needed before we can access anything for I2C, DO NOT REMOVE

	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_set_af(GPIOB, GPIO_AF4, GPIO8 | GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ,  GPIO8 | GPIO9);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);

	i2c_reset(I2C1);
	i2c_peripheral_disable(I2C1);

	// Set HSI as clock for I2C peripheral
	RCC_DCKCFGR2 |= (0x10 << RCC_DCKCFGR2_I2C1SEL_SHIFT);

	i2c_enable_analog_filter(I2C1);
	i2c_set_digital_filter(I2C1, 0); //Disabled

	/* HSI is at 16Mhz */
	i2c_set_speed(I2C1, i2c_speed_sm_100k, 16);
	i2c_enable_stretching(I2C1);
	i2c_set_7bit_addr_mode(I2C1);

	i2c_peripheral_enable(I2C1);
}

static void usart_setup(void)
{
    // In order to use our UART, we must enable the clock to it as well.
    rcc_periph_clock_enable(RCC_USART3);
    rcc_periph_clock_enable(RCC_GPIOD);

	/* Setup GPIO pins for USART3 transmit. */
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_af(GPIOD, GPIO_AF7, GPIO8);

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

static void gpio_setup() 
{
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
}

int main() 
{
    clock_setup();
    systick_setup();
	usart_setup();
    gpio_setup();
    i2c_setup();


    printf("FIRST OUTPUT\n");

	// We are getting WHO_AM_I register from LIS2DW12
	uint8_t addr = 0x19;
	uint8_t cmd = 0x0F;
	uint8_t data;

	while (1) 
	{
		i2c_transfer7(I2C1, addr, &cmd, 1, &data, 1);
		printf("Received this = %d\n", data);
        gpio_set(GPIOB, GPIO14);
		delay(1000);
		gpio_clear(GPIOB, GPIO14);
		delay(1000);
	}

	return 0;
}

