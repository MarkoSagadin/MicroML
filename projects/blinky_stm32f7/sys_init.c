#include <errno.h>
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include "utility.h"
#include "sys_init.h"

// Our clock frequency in MHz, it has to be set manually by programmer in clock setup
// It is used for calculating micros in utility.c
volatile uint8_t g_clock_mhz = 0;

void clock_setup()
{
    // First, let's ensure that our clock is running off the high-speed internal
    // oscillator (HSI) at 48MHz.
    rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_216MHZ]);

    g_clock_mhz = 216;     // Has to be the same as our clock in Mhz

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

    // RCC periphial clock enables
    // Enable clock for SPI1 peripheral and gpio pins
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_USART3);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOD);
}

void systick_setup() 
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

void gpio_setup() 
{
    // Our LEDs are connected to Port B pins 0, 7 and 14 so let's set them 
    // as output
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
}

