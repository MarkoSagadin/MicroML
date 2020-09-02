#include "utility.h"
#include "sys_init.h" //Needed because of g_clock_mhz

// Storage for our monotonic system clock.
// Note that it needs to be volatile since we're modifying it from an interrupt.
static volatile uint64_t _millis = 0;

/*!
 * @brief   Returns how long microcontroller has been running in microseconds
 *
 * @return  Time alive in milliseconds
 * @note    This value is incremented in sys_tick_handler
 */
uint64_t millis()
{
    return _millis;
}

/*!
 * @brief   Returns how long microcontroller has been running in microseconds
 *
 * @return  Time alive in microseconds
 *
 * @note    Explanation on implementation, we first get ms and turn them in us,
 *          then we get number of cycles left in systick timer, and turn that 
 *          into us. Last part might not make sense, but only because it was 
 *          simplified to avoid unnecessary math operations. 
 *
 *          Original equation was:
 *          number_of_cycles_in_ms = rcc_ahb_frequency/1000;
 *          cycles_in_systick = systick_get_value();
 *          time_in_us = (number_of_cycles_in_ms - cycles_in_systick)/(rcc_ahb_frequency/1000000);
 *          return (millis() * 1000) + time_in_us;
 */
uint64_t micros()
{
    return (millis() * 1000) + (1000 - (systick_get_value() / g_clock_mhz));
}

/*!
 * @brief                   This is our interrupt handler for the 
 *                          systick reload interrupt.
 *
 * @param[in] duration      In milliseconds
 *
 * @note    The full list of interrupt services routines that can be 
 *          implemented is listed in 
 *          libopencm3/include/libopencm3/stm32/f0/nvic.h
 */
void sys_tick_handler(void)
{
    // Increment our monotonic clock
    _millis++;
}

/*!
 * @brief                   Delay for a real number of milliseconds
 *
 * @param[in] duration      In milliseconds
 *
 * @note                    Blocks for specified duration
 */
void delay(uint64_t duration)
{
    const uint64_t until = millis() + duration;
    while (millis() < until);
}

/*!
 * @brief                   Delay for a real number of microseconds
 *
 * @param[in] duration      In microseconds
 *
 * @note                    Blocks for specified duration
 */
void delay_us(uint64_t duration)
{
    const uint64_t until = micros() + duration;
    while (micros() < until);
}
