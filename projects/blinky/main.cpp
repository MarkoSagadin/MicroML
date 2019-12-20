#include <errno.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>

extern "C" {
    void sys_tick_handler(void);
}

static void clock_setup() {
    // First, let's ensure that our clock is running off the high-speed internal
    // oscillator (HSI) at 48MHz.
    //rcc_clock_setup_in_hsi_out_48mhz();

    // Since our LED is on GPIO bank A, we need to enable
    // the peripheral clock to this GPIO bank in order to use it.
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOB);

    // In order to use our UART, we must enable the clock to it as well.
    rcc_periph_clock_enable(RCC_USART1);
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
    // Our test LED is connected to Port C pin 4, so let's set it as output
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
}

int main() 
{
    clock_setup();
    systick_setup();
    gpio_setup();

    // Toggle the LED on and off forever
    while (1) 
    {
        gpio_set(GPIOC, GPIO4);
        delay(1000);
        gpio_clear(GPIOC, GPIO4);
        delay(1000);
    }

    return 0;
}
