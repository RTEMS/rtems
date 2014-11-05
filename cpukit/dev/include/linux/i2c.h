/**
 * @file
 *
 * @brief RTEMS Port of Linux I2C API
 *
 * @ingroup I2CLinux
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _UAPI_LINUX_I2C_H
#define _UAPI_LINUX_I2C_H

#include <stdint.h>

/**
 * @defgroup I2CLinux Linux I2C User-Space API
 *
 * @ingroup I2C
 *
 * @brief RTEMS port of Linux I2C user-space API.
 *
 * Additional documentation is available through the Linux sources, see
 *
 * - /usr/src/linux/include/uapi/linux/i2c.h,
 * - /usr/src/linux/include/uapi/linux/i2c-dev.h, and
 * - /usr/src/linux/Documentation/i2c.
 *
 * @{
 */

/**
 * @name I2C Message Flags
 *
 * @{
 */

/**
 * @brief I2C message flag to indicate a 10-bit address.
 *
 * The controller must support this as indicated by the I2C_FUNC_10BIT_ADDR
 * functionality.
 *
 * @see i2c_msg.
 */
#define I2C_M_TEN 0x0010

/**
 * @brief I2C message flag to indicate a read transfer (from slave to master).
 *
 * @see i2c_msg.
 */
#define I2C_M_RD 0x0001

/**
 * @brief I2C message flag to signal a stop condition even if this is not the
 * last message.
 *
 * The controller must support this as indicated by the
 * @ref I2C_FUNC_PROTOCOL_MANGLING functionality.
 *
 * @see i2c_msg.
 */
#define I2C_M_STOP 0x8000

/**
 * @brief I2C message flag to omit start condition and slave address.
 *
 * The controller must support this as indicated by the
 * @ref I2C_FUNC_NOSTART functionality.
 *
 * @see i2c_msg.
 */
#define I2C_M_NOSTART 0x4000

/**
 * @brief I2C message flag to reverse the direction flag.
 *
 * The controller must support this as indicated by the
 * @ref I2C_FUNC_PROTOCOL_MANGLING functionality.
 *
 * @see i2c_msg.
 */
#define I2C_M_REV_DIR_ADDR 0x2000

/**
 * @brief I2C message flag to ignore a non-acknowledge.
 *
 * The controller must support this as indicated by the
 * @ref I2C_FUNC_PROTOCOL_MANGLING functionality.
 *
 * @see i2c_msg.
 */
#define I2C_M_IGNORE_NAK 0x1000

/**
 * @brief I2C message flag to omit a master acknowledge/non-acknowledge in a
 * read transfer.
 *
 * The controller must support this as indicated by the
 * @ref I2C_FUNC_PROTOCOL_MANGLING functionality.
 *
 * @see i2c_msg.
 */
#define I2C_M_NO_RD_ACK 0x0800

/**
 * @brief I2C message flag to indicate that the message data length is the
 * first received byte.
 *
 * The message data buffer must be large enough to store up to 32 bytes, the
 * initial length byte and the SMBus PEC (if used).  Initialize the message
 * length to one.  The message length is incremented by the count of received
 * data bytes.
 *
 * @see i2c_msg.
 */
#define I2C_M_RECV_LEN 0x0400

/** @} */

/**
 * @brief I2C transfer message.
 */
struct i2c_msg {
  /**
   * @brief The slave address.
   *
   * In case the @ref I2C_M_TEN flag is set, then this is a 10-bit address,
   * otherwise it is a 7-bit address.
   */
  uint16_t addr;

  /**
   * @brief The message flags.
   *
   * Valid flags are
   * - @ref I2C_M_TEN,
   * - @ref I2C_M_RD,
   * - @ref I2C_M_STOP,
   * - @ref I2C_M_NOSTART,
   * - @ref I2C_M_REV_DIR_ADDR,
   * - @ref I2C_M_IGNORE_NAK,
   * - @ref I2C_M_NO_RD_ACK, and
   * - @ref I2C_M_RECV_LEN.
   */
  uint16_t flags;

  /**
   * @brief The message data length in bytes.
   */
  uint16_t len;

  /**
   * @brief Pointer to the message data.
   */
  uint8_t *buf;
};

/**
 * @name I2C Controller Functionality
 *
 * @{
 */

#define I2C_FUNC_I2C 0x00000001
#define I2C_FUNC_10BIT_ADDR 0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING 0x00000004
#define I2C_FUNC_SMBUS_PEC 0x00000008
#define I2C_FUNC_NOSTART 0x00000010
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL 0x00008000
#define I2C_FUNC_SMBUS_QUICK 0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE 0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE 0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA 0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA 0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA 0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA 0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL 0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA 0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA 0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK 0x04000000
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK 0x08000000

#define I2C_FUNC_SMBUS_BYTE \
  (I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE)

#define I2C_FUNC_SMBUS_BYTE_DATA \
  (I2C_FUNC_SMBUS_READ_BYTE_DATA | I2C_FUNC_SMBUS_WRITE_BYTE_DATA)

#define I2C_FUNC_SMBUS_WORD_DATA \
  (I2C_FUNC_SMBUS_READ_WORD_DATA | I2C_FUNC_SMBUS_WRITE_WORD_DATA)

#define I2C_FUNC_SMBUS_BLOCK_DATA \
  (I2C_FUNC_SMBUS_READ_BLOCK_DATA | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)

#define I2C_FUNC_SMBUS_I2C_BLOCK \
  (I2C_FUNC_SMBUS_READ_I2C_BLOCK | I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

#define I2C_FUNC_SMBUS_EMUL \
  (I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA \
    | I2C_FUNC_SMBUS_WORD_DATA | I2C_FUNC_SMBUS_PROC_CALL \
    | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | I2C_FUNC_SMBUS_I2C_BLOCK \
    | I2C_FUNC_SMBUS_PEC)

/** @} */

/**
 * @brief Maximum SMBus data block count.
 */
#define I2C_SMBUS_BLOCK_MAX 32

/**
 * @brief SMBus data.
 */
union i2c_smbus_data {
  uint8_t byte;
  uint16_t word;
  uint8_t block[I2C_SMBUS_BLOCK_MAX + 2];
};

/**
 * @name SMBus Transfer Read and Write Markers
 *
 * @{
 */

#define I2C_SMBUS_READ 1

#define I2C_SMBUS_WRITE 0

/** @} */

/**
 * @name SMBus Transaction Types
 *
 * @{
 */

#define I2C_SMBUS_QUICK 0

#define I2C_SMBUS_BYTE 1

#define I2C_SMBUS_BYTE_DATA 2

#define I2C_SMBUS_WORD_DATA 3

#define I2C_SMBUS_PROC_CALL 4

#define I2C_SMBUS_BLOCK_DATA 5

#define I2C_SMBUS_I2C_BLOCK_BROKEN 6

#define I2C_SMBUS_BLOCK_PROC_CALL 7

#define I2C_SMBUS_I2C_BLOCK_DATA 8

/** @} */

/** @} */

#endif /* _UAPI_LINUX_I2C_H */
