#include <errno.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include "printf.h"
#include "utility.h"
#include "sys_init.h"

uint32_t interrupt_value = 0;
void exti15_10_isr(void)
{
    printf("Interrupt happened\n");

    interrupt_value = exti_get_flag_status(EXTI13);
    exti_reset_request(EXTI13);
}

int main() 
{
    clock_setup();
    systick_setup();
    gpio_setup();
	usart_setup();
    printf("FIRST OUTPUT\n");

    // Preparing interrupt, enable rcc clock first
    rcc_periph_clock_enable(RCC_SYSCFG);
    nvic_enable_irq(NVIC_EXTI15_10_IRQ);
    exti_select_source(EXTI13, GPIOC);
    exti_set_trigger(EXTI13, EXTI_TRIGGER_RISING);
    exti_enable_request(EXTI13);

    while (1) 
    {
        uint16_t value = gpio_get(GPIOC, GPIO13) >> 13;
        if (value)
        {
            gpio_set(GPIOB, GPIO7);
        }
        else
        {
            gpio_clear(GPIOB, GPIO7);
        }

        printf("value: %d\n", value);
        printf("interrupt value: %ld\n", interrupt_value >> 13);
        interrupt_value = 0;
        delay(500);
    }

    return 0;
}

