/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _DEV_SERIAL_SC16IS752_H
#define _DEV_SERIAL_SC16IS752_H

#include <sys/ioccom.h>

#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup SC16IS752 SC16IS752 Serial Device Driver
 *
 * @ingroup TermiostypesSupport
 */

typedef enum {
  SC16IS752_MODE_RS232,

  /* Enable RS485 mode */
  SC16IS752_MODE_RS485,

  /* Enable RS485 mode, enable the transmitter to control the #RTS pin */
  SC16IS752_MODE_RS485_RTS,

  /*
   * Enable RS485 mode, enable the transmitter to control the #RTS pin, invert
   * RTS signal (#RTS = 1 during transmission and #RTS = 0 during reception)
   */
  SC16IS752_MODE_RS485_RTS_INV
} sc16is752_mode;

typedef struct sc16is752_context sc16is752_context;

/**
 * @brief SC16IS752 device context.
 */
struct sc16is752_context {
  rtems_termios_device_context base;

  /**
   * @brief Writes a register.
   *
   * Internal handler.
   */
  int (*write_reg)(
    sc16is752_context *ctx,
    uint8_t addr,
    const uint8_t *data,
    size_t len
  );

  /**
   * @brief Reads a register.
   *
   * Internal handler.
   */
  int (*read_reg)(
    sc16is752_context *ctx,
    uint8_t addr,
    uint8_t *data,
    size_t len
  );

  /**
   * @brief Reads two registers.
   *
   * Internal handler.
   */
  int (*read_2_reg)(
    sc16is752_context *ctx,
    uint8_t addr_0,
    uint8_t addr_1,
    uint8_t data[2]
  );

  /**
   * @brief First open.
   *
   * Internal handler.
   */
  bool (*first_open)(sc16is752_context *ctx);

  /**
   * @brief Last close.
   *
   * Internal handler.
   */
  void (*last_close)(sc16is752_context *ctx);

  /**
   * @brief Shall install the interrupt handler.
   *
   * Must be initialized by the user before the device creation.
   */
  bool (*install_irq)(sc16is752_context *ctx);

  /**
   * @brief Shall remove the interrupt handler.
   *
   * Must be initialized by the user before the device creation.
   */
  void (*remove_irq)(sc16is752_context *ctx);

  /**
   * @brief Device mode.
   *
   * Must be initialized by the user before the device creation.
   */
  sc16is752_mode mode;

  /**
   * @brief Input frequency in Hertz (dependent on crystal, see XTAL1 and XTAL2
   * pins).
   *
   * Must be initialized by the user before the device creation.
   */
  uint32_t input_frequency;

  /**
   * @brief Corresponding Termios structure.
   *
   * Internal variable.
   */
  rtems_termios_tty *tty;

  /**
   * @brief Shadow Interrupt Enable Register (IER).
   *
   * Internal variable.
   */
  uint8_t ier;

  /**
   * @brief Characters placed into transmit FIFO.
   *
   * Internal variable.
   */
  uint8_t tx_in_progress;

  /**
   * @brief Count of free characters in the transmit FIFO.
   *
   * Internal variable.
   */
  uint8_t tx_fifo_free;

  /**
   * @brief Shadow Line Control Register (LCR).
   *
   * Internal variable.
   */
  uint8_t lcr;

  /**
   * @brief Shadow Extra Features Control Register (EFCR).
   *
   * Internal variable.
   */
  uint8_t efcr;
};

/**
 * @brief SC16IS752 SPI context.
 */
typedef struct {
  sc16is752_context base;

  /**
   * @brief The SPI bus device file descriptor.
   *
   * Internal variable.
   */
  int fd;

  /**
   * @brief The SPI device chip select.
   *
   * Must be initialized by the user before the call to sc16is752_spi_create().
   */
  uint8_t cs;

  /**
   * @brief The SPI bus speed in Hertz.
   *
   * Must be initialized by the user before the call to sc16is752_spi_create().
   */
  uint32_t speed_hz;

  /**
   * @brief The SPI bus device path.
   *
   * Must be initialized by the user before the call to sc16is752_spi_create().
   */
  const char *spi_path;
} sc16is752_spi_context;

/**
 * @brief SC16IS752 I2C context.
 */
typedef struct {
  sc16is752_context base;

  /**
   * @brief The I2C bus device file descriptor.
   *
   * Internal variable.
   */
  int fd;

  /**
   * @brief The I2C bus device path.
   *
   * Must be initialized before the call to sc16is752_i2c_create().
   */
  const char *bus_path;
} sc16is752_i2c_context;

const rtems_termios_device_handler sc16is752_termios_handler;

/**
 * @brief The interrupt handler for receive and transmit operations.
 *
 * @param[in] arg The device context.
 */
void sc16is752_interrupt_handler(void *arg);

/**
 * @brief Creates an SPI connected SC16IS752 device.
 *
 * @param[in] ctx The SPI SC16IS752 device context.
 * @param[in] device_path The device file path for the new device.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval other See rtems_termios_device_install().
 */
rtems_status_code sc16is752_spi_create(
  sc16is752_spi_context *ctx,
  const char            *device_path
);

/**
 * @brief Enables the sleep mode if non-zero, otherwise disables it.
 *
 * The sleep mode is disabled by default.
 */
#define SC16IS752_SET_SLEEP_MODE _IOW('d', 0, int)

/**
 * @brief Set the I/O Control bits except for the SRESET.
 *
 * Note that it will not be possible to set the SRESET. Otherwise the driver
 * might would have an undefined state.
 */
#define SC16IS752_SET_IOCONTROL _IOW('d', 1, uint8_t)

/**
 * @brief Set the I/O pins direction register.
 */
#define SC16IS752_SET_IODIR _IOW('d', 2, uint8_t)

/**
 * @brief Set the I/O pins state register.
 */
#define SC16IS752_SET_IOSTATE _IOW('d', 3, uint8_t)

/**
 * @brief Set the EFCR register.
 */
#define SC16IS752_SET_EFCR _IOW('d', 4, uint8_t)

/**
 * @brief Returns non-zero in case the sleep mode is enabled, otherwise zero.
 */
#define SC16IS752_GET_SLEEP_MODE _IOR('d', 0, int)

/**
 * @brief Read the I/O Control register.
 */
#define SC16IS752_GET_IOCONTROL _IOR('d', 1, uint8_t)

/**
 * @brief Read the I/O pins direction register.
 */
#define SC16IS752_GET_IODIR _IOR('d', 2, uint8_t)

/**
 * @brief Read the I/O pins state register.
 */
#define SC16IS752_GET_IOSTATE _IOR('d', 3, uint8_t)

/**
 * @brief Read the EFCR register.
 */
#define SC16IS752_GET_EFCR _IOR('d', 4, uint8_t)

/**
 * @brief Bits for the IOCONTROL register.
 * @{
 */
#define SC16IS752_IOCONTROL_SRESET (1u << 3)
#define SC16IS752_IOCONTROL_GPIO_3_0_OR_MODEM (1u << 2)
#define SC16IS752_IOCONTROL_GPIO_7_4_OR_MODEM (1u << 1)
#define SC16IS752_IOCONTROL_IOLATCH (1u << 0)
/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_SERIAL_SC16IS752_H */
