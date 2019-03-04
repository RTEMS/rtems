/**
 * @file
 * @ingroup stm32f4_i2c I2C Support
 * @brief I2C-module.
 */

/*
 * Copyright (c) 2013 Christian Mauderer.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/* The I2C-module can not run with libi2c. The reason for this is, that libi2c
 * needs a possibility to generate a stop condition separately. This controller
 * wants to generate the condition automatically when sending or receiving data.
 */

#ifndef LIBBSP_ARM_STM32F4_I2C_H
#define LIBBSP_ARM_STM32F4_I2C_H

#include <rtems.h>

#include <bsp/io.h>
#include <bsp/stm32f4.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup stm32f4_i2c I2C Support
 * @ingroup RTEMSBSPsARMSTM32F4
 * @brief I2C Module
 * @{
 */

typedef struct {
  /**
   * @brief The address of the slave without the read write bit.
   * A 7-Bit address should be placed in the bits [6..0]
   */
  uint16_t addr;
  /** @brief Read (true) or write (false) data */
  bool read;
  /** @brief Size of data to read or write */
  size_t len;
  /** @brief Buffer for data */
  uint8_t *buf;
} stm32f4_i2c_message;

typedef struct {
  volatile stm32f4_i2c *regs;
  size_t index;
  rtems_vector_number vector;
  rtems_id mutex;
  rtems_id task_id;
  uint8_t *data;
  uint8_t *last;
  size_t len;
  bool read;
  uint8_t addr_with_rw;
} stm32f4_i2c_bus_entry;

/** @brief Initialise the i2c module. */
rtems_status_code stm32f4_i2c_init(stm32f4_i2c_bus_entry *e);

/** @brief Process a i2c message */
rtems_status_code stm32f4_i2c_process_message(
  stm32f4_i2c_bus_entry *e,
  stm32f4_i2c_message *msg
);

/** @brief Set another baud rate than the default one */
rtems_status_code stm32f4_i2c_set_bitrate(
  stm32f4_i2c_bus_entry *e,
  uint32_t br
);

extern stm32f4_i2c_bus_entry *const stm32f4_i2c1;
extern stm32f4_i2c_bus_entry *const stm32f4_i2c2;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_STM32F4_I2C_H */
