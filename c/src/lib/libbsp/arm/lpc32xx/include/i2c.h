/**
 * @file
 *
 * @ingroup lpc32xx_i2c
 *
 * @brief I2C support API.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC32XX_I2C_H
#define LIBBSP_ARM_LPC32XX_I2C_H

#include <rtems.h>

#include <bsp/lpc32xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc32xx_i2c I2C Support
 *
 * @ingroup lpc32xx
 *
 * @brief I2C Support
 *
 * All writes and reads will be performed in master mode.  Exclusive bus access
 * will be assumed.
 *
 * @{
 */

/**
 * @name I2C Clock Control Register (I2CCLK_CTRL)
 *
 * @{
 */

#define I2CCLK_1_EN BSP_BIT32(0)
#define I2CCLK_2_EN BSP_BIT32(1)
#define I2CCLK_1_HIGH_DRIVE BSP_BIT32(2)
#define I2CCLK_2_HIGH_DRIVE BSP_BIT32(3)
#define I2CCLK_USB_HIGH_DRIVE BSP_BIT32(4)

/** @} */

/**
 * @name I2C TX Data FIFO Register (I2Cn_TX)
 *
 * @{
 */

#define I2C_TX_READ BSP_BIT32(0)
#define I2C_TX_ADDR(val) BSP_FLD32(val, 1, 7)
#define I2C_TX_START BSP_BIT32(8)
#define I2C_TX_STOP BSP_BIT32(9)

/** @} */

/**
 * @name I2C Status Register (I2Cn_STAT)
 *
 * @{
 */

#define I2C_STAT_TDI BSP_BIT32(0)
#define I2C_STAT_AFI BSP_BIT32(1)
#define I2C_STAT_NAI BSP_BIT32(2)
#define I2C_STAT_DRMI BSP_BIT32(3)
#define I2C_STAT_DRSI BSP_BIT32(4)
#define I2C_STAT_ACTIVE BSP_BIT32(5)
#define I2C_STAT_SCL BSP_BIT32(6)
#define I2C_STAT_SDA BSP_BIT32(7)
#define I2C_STAT_RFF BSP_BIT32(8)
#define I2C_STAT_RFE BSP_BIT32(9)
#define I2C_STAT_TFF BSP_BIT32(10)
#define I2C_STAT_TFE BSP_BIT32(11)
#define I2C_STAT_TFFS BSP_BIT32(12)
#define I2C_STAT_TFES BSP_BIT32(13)

/** @} */

/**
 * @name I2C Control Register (I2Cn_CTRL)
 *
 * @{
 */

#define I2C_CTRL_TDIE BSP_BIT32(0)
#define I2C_CTRL_AFIE BSP_BIT32(1)
#define I2C_CTRL_NAIE BSP_BIT32(2)
#define I2C_CTRL_DRMIE BSP_BIT32(3)
#define I2C_CTRL_DRSIE BSP_BIT32(4)
#define I2C_CTRL_RFFIE BSP_BIT32(5)
#define I2C_CTRL_RFDAIE BSP_BIT32(6)
#define I2C_CTRL_TFFIO BSP_BIT32(7)
#define I2C_CTRL_RESET BSP_BIT32(8)
#define I2C_CTRL_SEVEN BSP_BIT32(9)
#define I2C_CTRL_TFFSIE BSP_BIT32(10)

/** @} */

/**
 * @brief Initializes the I2C module @a i2c.
 *
 * Valid @a clock_in_hz values are 100000 and 400000.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid @a i2c value.
 * @retval RTEMS_INVALID_CLOCK Invalid @a clock_in_hz value.
 */
rtems_status_code lpc32xx_i2c_init(
  volatile lpc32xx_i2c *i2c,
  unsigned clock_in_hz
);

/**
 * @brief Resets the I2C module @a i2c.
 */
void lpc32xx_i2c_reset(volatile lpc32xx_i2c *i2c);

/**
 * @brief Sets the I2C module @a i2c clock.
 *
 * Valid @a clock_in_hz values are 100000 and 400000.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_CLOCK Invalid @a clock_in_hz value.
 */
rtems_status_code lpc32xx_i2c_clock(
  volatile lpc32xx_i2c *i2c,
  unsigned clock_in_hz
);

/**
 * @brief Starts a write transaction on the I2C module @a i2c.
 *
 * The address parameter @a addr must not contain the read/write bit.
 *
 * The error status may be delayed to the next
 * lpc32xx_i2c_write_with_optional_stop() due to controller flaws.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 */
rtems_status_code lpc32xx_i2c_write_start(
  volatile lpc32xx_i2c *i2c,
  unsigned addr
);

/**
 * @brief Writes data via the I2C module @a i2c with optional stop.
 *
 * The error status may be delayed to the next
 * lpc32xx_i2c_write_with_optional_stop() due to controller flaws.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 */
rtems_status_code lpc32xx_i2c_write_with_optional_stop(
  volatile lpc32xx_i2c *i2c,
  const uint8_t *out,
  size_t n,
  bool stop
);

/**
 * @brief Starts a read transaction on the I2C module @a i2c.
 *
 * The address parameter @a addr must not contain the read/write bit.
 *
 * The error status may be delayed to the next
 * lpc32xx_i2c_read_with_optional_stop() due to controller flaws.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 */
rtems_status_code lpc32xx_i2c_read_start(
  volatile lpc32xx_i2c *i2c,
  unsigned addr
);

/**
 * @brief Reads data via the I2C module @a i2c with optional stop.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 * @retval RTEMS_NOT_IMPLEMENTED Stop is @a false.
 */
rtems_status_code lpc32xx_i2c_read_with_optional_stop(
  volatile lpc32xx_i2c *i2c,
  uint8_t *in,
  size_t n,
  bool stop
);

/**
 * @brief Writes and reads data via the I2C module @a i2c.
 *
 * This will be one bus transaction.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 */
rtems_status_code lpc32xx_i2c_write_and_read(
  volatile lpc32xx_i2c *i2c,
  unsigned addr,
  const uint8_t *out,
  size_t out_size,
  uint8_t *in,
  size_t in_size
);

/**
 * @brief Writes data via the I2C module @a i2c.
 *
 * This will be one bus transaction.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 */
static inline rtems_status_code lpc32xx_i2c_write(
  volatile lpc32xx_i2c *i2c,
  unsigned addr,
  const uint8_t *out,
  size_t out_size
)
{
  return lpc32xx_i2c_write_and_read(i2c, addr, out, out_size, NULL, 0);
}

/**
 * @brief Reads data via the I2C module @a i2c.
 *
 * This will be one bus transaction.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Received a NACK from the slave.
 */
static inline rtems_status_code lpc32xx_i2c_read(
  volatile lpc32xx_i2c *i2c,
  unsigned addr,
  uint8_t *in,
  size_t in_size
)
{
  return lpc32xx_i2c_write_and_read(i2c, addr, NULL, 0, in, in_size);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_I2C_H */
