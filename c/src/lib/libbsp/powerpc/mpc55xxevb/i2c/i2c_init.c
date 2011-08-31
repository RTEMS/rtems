/*===============================================================*\
| Project: RTEMS support for GWLCFM                               |
+-----------------------------------------------------------------+
|                    Copyright (c) 2010                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the low level MPC5516 I2C driver parameters  |
\*===============================================================*/

#include <libchip/i2c-2b-eeprom.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/mpc83xx_i2cdrv.h>

#if MPC55XX_CHIP_TYPE / 10 == 551
  static mpc83xx_i2c_desc_t mpc55xx_i2c_bus = {
    .bus_desc = {
      .ops = &mpc83xx_i2c_ops,
      .size = sizeof(mpc55xx_i2c_bus),
    },
    .softc = {
      .reg_ptr = (m83xxI2CRegisters_t *) 0xfff88000,
      .initialized = FALSE,
      .irq_number = MPC55XX_IRQ_I2C(0),
      .base_frq = 0
    }
  };

  rtems_status_code bsp_register_i2c(void)
  {
    int rv = 0;
    int busno = 0;

    rtems_libi2c_initialize ();

    mpc55xx_i2c_bus.softc.base_frq = bsp_clock_speed;
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
