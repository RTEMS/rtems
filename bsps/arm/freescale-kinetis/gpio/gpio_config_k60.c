/**
 * @file gpio_config.c
 *
 * @ingroup kinetis_gpio
 *
 * @brief FreeScale Kinetis GPIO Configuration.
 */

#include <bsp/gpio.h>

rtems_status_code kinetis_gpio_select_uart(void);

#define UART_PIN_COUNT 4
#define SPI_PIN_COUNT 5

rtems_gpio_specific_data alt_func_def[] = {
  {.io_function = FN_0, .pin_data = NULL},
  {.io_function = FN_1, .pin_data = NULL},
  {.io_function = FN_2, .pin_data = NULL},
  {.io_function = FN_3, .pin_data = NULL},
  {.io_function = FN_4, .pin_data = NULL},
  {.io_function = FN_5, .pin_data = NULL},
  {.io_function = FN_6, .pin_data = NULL},
  {.io_function = FN_7, .pin_data = NULL}
};


const rtems_gpio_pin_conf uart_config[UART_PIN_COUNT] = {
  // { /* UART0 RXD - PTA1 */
  //   .pin_number = 1,
  //   .function = BSP_SPECIFIC,
  //   .pull_mode = NO_PULL_RESISTOR,
  //   .interrupt = NULL,
  //   .output_enabled = FALSE,
  //   .logic_invert = FALSE,
  //   .bsp_specific = &alt_func_def[2]
  // },
  // { /* UART0 TXD - PTA2 */
  //   .pin_number = 2,
  //   .function = BSP_SPECIFIC,
  //   .pull_mode = NO_PULL_RESISTOR,
  //   .interrupt = NULL,
  //   .output_enabled = FALSE,
  //   .logic_invert = FALSE,
  //   .bsp_specific = &alt_func_def[3]
  // },
  
  { /* UART3 TXD - PTE4 */
    .pin_number = 4,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[3]
  },
  { /* UART3 RXD - PTE5 */
    .pin_number = 5,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[3]
  },

  { /* UART4 RXD - PTC14 */
    .pin_number = 14,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[3]
  },
  { /* UART4 TXD - PTC15 */
    .pin_number = 15,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[3]
  }
};

const rtems_gpio_pin_conf spi_config[SPI_PIN_COUNT] = {
  { /* spi_miso */

  },
  { /* spi_mosi */

  },
  { /* spi_sclk */

  },
  { /* spi_ce_0 */

  },
  { /* spi_ce_1 */

  }
};

rtems_status_code kinetis_gpio_select_uart(void)
{
    return rtems_gpio_multi_select(uart_config, UART_PIN_COUNT);
}