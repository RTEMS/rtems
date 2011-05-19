/**
 * @file
 *
 * @ingroup lpc24xx_libi2c
 *
 * @brief LibI2C bus driver for the I2C modules.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <bsp/i2c.h>
#include <bsp/irq.h>

#ifdef LPC24XX_CONFIG_I2C_0
  static const lpc24xx_pin_range lpc24xx_i2c_pins_0 [] = {
    LPC24XX_PIN_I2C_0_SDA,
    LPC24XX_PIN_I2C_0_SCL,
    LPC24XX_PIN_TERMINAL
  };

  static lpc24xx_i2c_bus_entry lpc24xx_i2c_entry_0 = {
    .bus = {
      .ops = &lpc24xx_i2c_ops,
      .size = sizeof(lpc24xx_i2c_bus_entry)
    },
    .regs = (volatile lpc24xx_i2c *) I2C0_BASE_ADDR,
    .index = 0,
    .pins = &lpc24xx_i2c_pins_0 [0],
    .vector = LPC24XX_IRQ_I2C_0
  };

  rtems_libi2c_bus_t * const lpc24xx_i2c_0 =
    &lpc24xx_i2c_entry_0.bus;
#endif

#ifdef LPC24XX_CONFIG_I2C_1
  static const lpc24xx_pin_range lpc24xx_i2c_pins_1 [] = {
    LPC24XX_PIN_I2C_1_SDA_P0_19,
    LPC24XX_PIN_I2C_1_SCL_P0_20,
    LPC24XX_PIN_TERMINAL
  };

  static lpc24xx_i2c_bus_entry lpc24xx_i2c_entry_1 = {
    .bus = {
      .ops = &lpc24xx_i2c_ops,
      .size = sizeof(lpc24xx_i2c_bus_entry)
    },
    .regs = (volatile lpc24xx_i2c *) I2C1_BASE_ADDR,
    .index = 1,
    .pins = &lpc24xx_i2c_pins_1 [0],
    .vector = LPC24XX_IRQ_I2C_1
  };

  rtems_libi2c_bus_t * const lpc24xx_i2c_1 =
    &lpc24xx_i2c_entry_1.bus;
#endif

#ifdef LPC24XX_CONFIG_I2C_2
  static const lpc24xx_pin_range lpc24xx_i2c_pins_2 [] = {
    LPC24XX_PIN_I2C_2_SDA_P0_10,
    LPC24XX_PIN_I2C_2_SCL_P0_11,
    LPC24XX_PIN_TERMINAL
  };

  static lpc24xx_i2c_bus_entry lpc24xx_i2c_entry_2 = {
    .bus = {
      .ops = &lpc24xx_i2c_ops,
      .size = sizeof(lpc24xx_i2c_bus_entry)
    },
    .regs = (volatile lpc24xx_i2c *) I2C2_BASE_ADDR,
    .index = 2,
    .pins = &lpc24xx_i2c_pins_2 [0],
    .vector = LPC24XX_IRQ_I2C_2
  };

  rtems_libi2c_bus_t * const lpc24xx_i2c_2 =
    &lpc24xx_i2c_entry_2.bus;
#endif
