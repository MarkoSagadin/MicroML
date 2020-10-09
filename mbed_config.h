/*
 * mbed SDK
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Automatically generated configuration file.
// DO NOT EDIT, content will be overwritten.

#ifndef __MBED_CONFIG_DATA__
#define __MBED_CONFIG_DATA__

// Configuration parameters
#define CLOCK_SOURCE                                                      USE_PLL_HSE_EXTC|USE_PLL_HSI            // set by target:NUCLEO_F767ZI
#define LPTICKER_DELAY_TICKS                                              0                                       // set by target:NUCLEO_F767ZI
#define MBED_CONF_APP_MAIN_STACK_SIZE                                     65536                                   // set by application
#define MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE                          256                                     // set by library:drivers
#define MBED_CONF_DRIVERS_UART_SERIAL_TXBUF_SIZE                          256                                     // set by library:drivers
#define MBED_CONF_PLATFORM_CRASH_CAPTURE_ENABLED                          1                                       // set by library:platform[NUCLEO_F767ZI]
#define MBED_CONF_PLATFORM_CTHUNK_COUNT_MAX                               8                                       // set by library:platform
#define MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE                       9600                                    // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_ALL_THREADS_INFO                         0                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_FILENAME_CAPTURE_ENABLED                 0                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_HIST_ENABLED                             0                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_HIST_SIZE                                4                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_REBOOT_MAX                               1                                       // set by library:platform
#define MBED_CONF_PLATFORM_FATAL_ERROR_AUTO_REBOOT_ENABLED                1                                       // set by library:platform[NUCLEO_F767ZI]
#define MBED_CONF_PLATFORM_FORCE_NON_COPYABLE_ERROR                       0                                       // set by library:platform
#define MBED_CONF_PLATFORM_MAX_ERROR_FILENAME_LEN                         16                                      // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_CONSOLE_OUTPUT                  UART                                    // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_64_BIT                   1                                       // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FILE_STREAM              1                                       // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FLOATING_POINT           1                                       // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_SET_FLOATING_POINT_MAX_DECIMALS 6                                       // set by library:platform
#define MBED_CONF_PLATFORM_POLL_USE_LOWPOWER_TIMER                        0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_BAUD_RATE                                9600                                    // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_BUFFERED_SERIAL                          0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES                         0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_TTY_NEWLINES                     0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT                            1                                       // set by library:platform
#define MBED_CONF_PLATFORM_USE_MPU                                        1                                       // set by library:platform
#define MBED_CONF_TARGET_BOOT_STACK_SIZE                                  0x1000                                  // set by target:Target
#define MBED_CONF_TARGET_CONSOLE_UART                                     1                                       // set by target:Target
#define MBED_CONF_TARGET_DEEP_SLEEP_LATENCY                               3                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_FLASH_DUAL_BANK                                  0                                       // set by target:NUCLEO_F767ZI
#define MBED_CONF_TARGET_INIT_US_TICKER_AT_BOOT                           1                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_LPTICKER_LPTIM                                   1                                       // set by target:NUCLEO_F767ZI
#define MBED_CONF_TARGET_LPTICKER_LPTIM_CLOCK                             1                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_LPUART_CLOCK_SOURCE                              USE_LPUART_CLK_LSE|USE_LPUART_CLK_PCLK1 // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_LSE_AVAILABLE                                    1                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_MPU_ROM_END                                      0x0fffffff                              // set by target:Target
#define MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE                   ETHERNET                                // set by target:NUCLEO_F767ZI
#define MBED_CONF_TARGET_TICKLESS_FROM_US_TICKER                          0                                       // set by target:Target
#define STM32_D11_SPI_ETHERNET_PIN                                        PA_7                                    // set by target:NUCLEO_F767ZI
// Macros
#define NDEBUG                                                                                                    // defined by application
#define TF_LITE_STATIC_MEMORY                                                                                     // defined by application

#endif
