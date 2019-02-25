/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief SPI support API.
 *
 * Based on bsps/m68k/gen68360/spi/m360_spi.h
 */

/*
 * Copyright (c) 2018 Pierre-Louis Garnier <garnie_a@epita.fr>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_BEAGLE_SPI_H
#define LIBBSP_ARM_BEAGLE_SPI_H

#include <bsp.h>
#include <rtems/libi2c.h>
#include <rtems/irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BBB_SPI_TIMEOUT 1000

#define BBB_SPI_0_BUS_PATH "/dev/spi-0"

#define BBB_SPI_0_IRQ AM335X_INT_SPI0INT

typedef enum {
  SPI0,
  SPI1,
  SPI_COUNT
} bbb_spi_id_t;



typedef struct BEAGLE_SPI_BufferDescriptor_ {
    unsigned short      status;
    unsigned short      length;
    volatile void       *buffer;
} BEAGLE_SPI_BufferDescriptor_t;

typedef struct beagle_spi_softc {
  int                     initialized;
  rtems_id                task_id;
  uintptr_t               regs_base;
  rtems_vector_number     irq;
} beagle_spi_softc_t;

typedef struct {
  rtems_libi2c_bus_t  bus_desc;
  beagle_spi_softc_t softc;
} beagle_spi_desc_t;

/*
 * Initialize the driver
 *
 * Returns: o = ok or error code
 */
rtems_status_code beagle_spi_init
(
 rtems_libi2c_bus_t *bh                  /* bus specifier structure        */
);

/*
 * Receive some bytes from SPI device
 *
 * Returns: number of bytes received or (negative) error code
 */
int beagle_spi_read_bytes
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to store bytes          */
 int len                                 /* number of bytes to receive     */
);

/*
 * Send some bytes to SPI device
 *
 * Returns: number of bytes sent or (negative) error code
 */
int beagle_spi_write_bytes
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to send                 */
 int len                                 /* number of bytes to send        */
);

/*
 * Set SPI to desired baudrate/clock mode/character mode
 *
 * Returns: rtems_status_code
 */
rtems_status_code beagle_spi_set_tfr_mode
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 const rtems_libi2c_tfr_mode_t *tfr_mode /* transfer mode info             */
);

/*
 * Perform selected ioctl function for SPI
 *
 * Returns: rtems_status_code
 */
int beagle_spi_ioctl
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 int                 cmd,                /* ioctl command code             */
 void               *arg                 /* additional argument array      */
);

/*
 * Register SPI bus and devices
 *
 * Returns: Bus number or error code
 */
rtems_status_code bsp_register_spi
(
  const char         *bus_path,
  uintptr_t           register_base,
  rtems_vector_number irq
);

static inline rtems_status_code bbb_register_spi_0(void)
{
  return bsp_register_spi(
    BBB_SPI_0_BUS_PATH,
    AM335X_SPI0_BASE,
    BBB_SPI_0_IRQ
  );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_SPI_H */
