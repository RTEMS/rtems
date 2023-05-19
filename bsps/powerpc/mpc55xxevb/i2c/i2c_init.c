/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS support for GWLCFM
 *
 * This file contains the low level MPC5516 I2C driver parameters.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <libchip/i2c-2b-eeprom.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <mpc83xx/mpc83xx_i2cdrv.h>

#if MPC55XX_CHIP_FAMILY == 551
  static void i2c_probe(mpc83xx_i2c_softc_t *self)
  {
    self->base_frq = bsp_clock_speed;
  }

  static mpc83xx_i2c_desc_t mpc55xx_i2c_bus = {
    .bus_desc = {
      .ops = &mpc83xx_i2c_ops,
      .size = sizeof(mpc55xx_i2c_bus),
    },
    .softc = {
      .reg_ptr = (m83xxI2CRegisters_t *) 0xfff88000,
      .initialized = FALSE,
      .irq_number = MPC55XX_IRQ_I2C(0),
      .base_frq = 0,
      .probe = i2c_probe
    }
  };

  rtems_status_code bsp_register_i2c(void)
  {
    int rv = 0;
    int busno = 0;

    rtems_libi2c_initialize ();

    busno = rtems_libi2c_register_bus(
      "/dev/i2c1",
      &mpc55xx_i2c_bus.bus_desc
    );
    if (busno < 0) {
      return RTEMS_IO_ERROR;
    }

    #ifdef RTEMS_BSP_I2C_EEPROM_DEVICE_NAME
      rv = rtems_libi2c_register_drv(
        RTEMS_BSP_I2C_EEPROM_DEVICE_NAME,
        i2c_2b_eeprom_driver_descriptor,
        busno,
        0x51
      );
      if (rv < 0) {
        return RTEMS_IO_ERROR;
      }
    #endif

    return RTEMS_SUCCESSFUL;
  }
#endif
