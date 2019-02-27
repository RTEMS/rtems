/**
 * @file
 *
 * @ingroup raspberrypi_i2c
 *
 * @brief Raspberry Pi specific I2C definitions.
 */

/*
 *  Copyright (c) 2014-2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_I2C_H
#define LIBBSP_ARM_RASPBERRYPI_I2C_H

#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @name I2C constants.
 *
 * @{
 */

/**
 * @brief BSC controller core clock rate in Hz.
 *
 * This is set to 150 MHz as per the BCM2835 datasheet.
 */
#define BSC_CORE_CLK_HZ 150000000

/**
 * @brief Default bus clock.
 *
 * This sets the bus with a 100 kHz clock speed.
 */
#define DEFAULT_BUS_CLOCK 100000

/** @} */

/**
 * @name  I2C directives.
 *
 * @{
 */

/**
 * @brief Setups the Raspberry Pi GPIO header to activate the BSC I2C bus.
 */
extern void rpi_i2c_init(void);

/**
 * @brief Registers the Raspberry Pi BSC I2C bus with the
 *        Linux I2C User-Space API.
 *
 * @param[in] bus_path Path to the bus device file.
 * @param[in] bus_clock Bus clock in Hz.
 *
 * @retval 0 Bus registered successfully.
 * @retval <0 Could not register the bus. The return value is a negative
 *            errno code.
 */
extern int rpi_i2c_register_bus(
  const char *bus_path,
  uint32_t bus_clock
);

/**
 * @brief Setups the Raspberry Pi BSC I2C bus (located on the GPIO header)
 *        on the "/dev/i2c" device file, using the default bus clock.
 *
 * @retval 0 Bus configured and registered successfully.
 * @retval <0 See @see rpi_i2c_register_bus().
 */
static inline int rpi_setup_i2c_bus(void)
{
  rpi_i2c_init();

  return rpi_i2c_register_bus("/dev/i2c", DEFAULT_BUS_CLOCK);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_RASPBERRYPI_I2C_H */
