/**
 * @file
 *
 * @ingroup raspberrypi_gpio
 *
 * @brief Raspberry PI 1 rev2 GPIO interface definitions.
 */

/*
 *  Copyright (c) 2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define JTAG_PIN_COUNT 5
#define SPI_PIN_COUNT 5
#define I2C_PIN_COUNT 2

const rtems_gpio_pin_conf jtag_config[JTAG_PIN_COUNT] = {
  { /*arm_tdi */
    .pin_number = 4,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[5]
  },
  { /* arm_trst */
    .pin_number = 22,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[4]
  },
  { /* arm_tdo */
    .pin_number = 24,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[4]
  },
  { /* arm_tck */
    .pin_number = 25,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[4]
  },
  { /* arm_tms */
    .pin_number = 27,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[4]
  }
};

const rtems_gpio_pin_conf spi_config[SPI_PIN_COUNT] = {
  { /* spi_miso */
    .pin_number = 7,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  },
  { /* spi_mosi */
    .pin_number = 8,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  },
  { /* spi_sclk */
    .pin_number = 9,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  },
  { /* spi_ce_0 */
    .pin_number = 10,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  },
  { /* spi_ce_1 */
    .pin_number = 11,
    .function = BSP_SPECIFIC,
    .pull_mode = NO_PULL_RESISTOR,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  }
};

const rtems_gpio_pin_conf i2c_config[I2C_PIN_COUNT] = {
  { /* i2c_sda */
    .pin_number = 2,
    .function = BSP_SPECIFIC,
    .pull_mode = PULL_UP,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  },
  { /* i2c_scl */
    .pin_number = 3,
    .function = BSP_SPECIFIC,
    .pull_mode = PULL_UP,
    .interrupt = NULL,
    .output_enabled = FALSE,
    .logic_invert = FALSE,
    .bsp_specific = &alt_func_def[0]
  }
};
