/**
 * @file
 *
 * @ingroup raspberrypi_spi
 *
 * @brief Raspberry Pi specific SPI definitions.
 */

/*
 *  Copyright (c) 2014-2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_SPI_H
#define LIBBSP_ARM_RASPBERRYPI_SPI_H

#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @name SPI constants.
 *
 * @{
 */

/**
 * @brief GPU processor core clock rate in Hz.
 *
 * Unless configured otherwise on a "config.txt" file present on the SD card
 * the GPU defaults to 250 MHz. Currently only 250 MHz is supported.
 */

/* TODO: It would be nice if this value could be probed at startup, probably
 *       using the Mailbox interface since the usual way of setting this on
 *       the hardware is through a "config.txt" text file on the SD card.
 *       Having this setup on the configure.ac script would require changing
 *       the same setting on two different places. */
#define GPU_CORE_CLOCK_RATE 250000000

/** @} */

/**
 * @name SPI directives.
 *
 * @{
 */

/**
 * @brief Setups the Raspberry Pi SPI bus (located on the GPIO header)
 *        on the "/dev/spi" device file, and registers the bus on the
 *        libi2c API.
 *
 * @param[in] bidirectional_mode If TRUE sets the SPI bus to use 2-wire SPI,
 *                               where the MOSI data line doubles as the
 *                               slave out (SO) and slave in (SI) data lines.
 *                               If FALSE the bus defaults to the usual
 *                               3-wire SPI, with 2 separate data lines
 *                               (MOSI and MISO).
 *
 * @retval Returns libi2c bus number.
 * @retval <0 Could not register the bus. See @see rtems_libi2c_register_bus().
 */
extern int rpi_spi_init(bool bidirectional_mode);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_RASPBERRYPI_SPI_H */
