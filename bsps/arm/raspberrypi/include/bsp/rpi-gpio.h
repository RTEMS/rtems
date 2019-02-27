/**
 * @file
 *
 * @ingroup raspberrypi_gpio
 *
 * @brief Raspberry Pi specific GPIO definitions.
 */

/*
 *  Copyright (c) 2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_RPI_GPIO_H
#define LIBBSP_ARM_RASPBERRYPI_RPI_GPIO_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief  Raspberry Pi GPIO functions.
 */
#define RPI_DIGITAL_IN  0
#define RPI_DIGITAL_OUT 1
#define RPI_ALT_FUNC_0  4
#define RPI_ALT_FUNC_1  5
#define RPI_ALT_FUNC_2  6
#define RPI_ALT_FUNC_3  7
#define RPI_ALT_FUNC_4  3
#define RPI_ALT_FUNC_5  2
#define RPI_ALT_FUNC_MASK 7

/**
 * @brief Setups a JTAG interface.
 *
 * @retval RTEMS_SUCCESSFUL JTAG interface successfully configured.
 * @retval * At least one of the required pins is currently
 *            occupied, @see rtems_gpio_request_pin_group().
 */
extern rtems_status_code rpi_gpio_select_jtag(void);

/**
 * @brief Setups a SPI interface.
 *
 * @retval RTEMS_SUCCESSFUL SPI interface successfully configured.
 * @retval * At least one of the required pins is currently
 *            occupied, @see rtems_gpio_request_pin_group().
 */
extern rtems_status_code rpi_gpio_select_spi(void);

/**
 * @brief Setups a I2C interface.
 *
 * @retval RTEMS_SUCCESSFUL I2C interface successfully configured.
 * @retval * At least one of the required pins is currently
 *            occupied, @see rtems_gpio_request_pin_group().
 */
extern rtems_status_code rpi_gpio_select_i2c(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_RASPBERRYPI_RPI_GPIO_H */
