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

#include <bspopts.h>
#include <bsp/i2c.h>
#include <bsp/irq.h>

#ifdef STM32F4_ENABLE_I2C1
  static stm32f4_i2c_bus_entry stm32f4_i2c1_entry = {
    .regs = STM32F4_I2C1,
    .index = 0,
    .vector = STM32F4_IRQ_I2C1_EV,
  };

  stm32f4_i2c_bus_entry *const stm32f4_i2c1 = &stm32f4_i2c1_entry;
#endif

#ifdef STM32F4_ENABLE_I2C2
  static stm32f4_i2c_bus_entry stm32f4_i2c2_entry = {
    .regs = STM32F4_I2C2,
    .index = 1,
    .vector = STM32F4_IRQ_I2C2_EV,
  };

  stm32f4_i2c_bus_entry *const stm32f4_i2c2 = &stm32f4_i2c2_entry;
#endif
