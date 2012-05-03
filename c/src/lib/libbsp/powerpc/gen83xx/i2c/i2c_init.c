/*===============================================================*\
| Project: RTEMS support for MPC83xx                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
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
| this file contains the low level MPC83xx I2C driver parameters  |
\*===============================================================*/


#include <mpc83xx/mpc83xx_i2cdrv.h>
#include <libchip/i2c-2b-eeprom.h>
#include <bsp/irq.h>
#include <bsp.h>

static mpc83xx_i2c_desc_t mpc83xx_i2c_bus_tbl[2] = {
  /* first channel */
  {
    {/* public fields */
      .ops = &mpc83xx_i2c_ops,
      .size = sizeof(mpc83xx_i2c_bus_tbl[0]),
    },
    { /* our private fields */
      .reg_ptr = &mpc83xx.i2c[0],
      .initialized = FALSE,
      .irq_number = BSP_IPIC_IRQ_I2C1,
      .base_frq = 0 /* will be set during initiailization */
    }
  },
  /* second channel */
  {
    { /* public fields */
      .ops = &mpc83xx_i2c_ops,
      .size = sizeof(mpc83xx_i2c_bus_tbl[1]),
    },
    { /* our private fields */
      .reg_ptr = &mpc83xx.i2c[1],
      .initialized = FALSE,
      .irq_number = BSP_IPIC_IRQ_I2C2,
      .base_frq = 0 /* will be set during initiailization */
    }
  }
};

rtems_libi2c_bus_t *mpc83xx_i2c_bus_descriptor[2] = {
  &mpc83xx_i2c_bus_tbl[0].bus_desc,
  &mpc83xx_i2c_bus_tbl[1].bus_desc
};

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code bsp_register_i2c
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   register I2C busses and devices                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void                                    /* <none>                         */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0 or error code                                                        |
\*=========================================================================*/

{
  int ret_code;
  int i2c1_busno,i2c2_busno;

  /*
   * init I2C library (if not already done)
   */
  rtems_libi2c_initialize ();

  /*
   * update input frequency of I2c modules into descriptor
   */
  /*
   * I2C1 is clocked with TSEC 1
   */
  if (((mpc83xx.clk.sccr >> (31-1)) & 0x03) > 0) {
    mpc83xx_i2c_bus_tbl[0].softc.base_frq =
      (BSP_bus_frequency
       /((mpc83xx.clk.sccr >> (31-1)) & 0x03));
  }

  mpc83xx_i2c_bus_tbl[1].softc.base_frq = BSP_bus_frequency;
  /*
   * register first I2C bus
   */
  ret_code = rtems_libi2c_register_bus("/dev/i2c1",
				       mpc83xx_i2c_bus_descriptor[0]);
  if (ret_code < 0) {
    return -ret_code;
  }
  i2c1_busno = ret_code;
  /*
   * register second I2C bus
   */
  ret_code = rtems_libi2c_register_bus("/dev/i2c2",
				       mpc83xx_i2c_bus_descriptor[1]);
  if (ret_code < 0) {
    return -ret_code;
  }
  i2c2_busno = ret_code;

#ifdef RTEMS_BSP_I2C_EEPROM_DEVICE_NAME

  /*
   * register EEPROM to bus 1, Address 0x50
   */
  ret_code = rtems_libi2c_register_drv(RTEMS_BSP_I2C_EEPROM_DEVICE_NAME,
				       i2c_2b_eeprom_driver_descriptor,
				       i2c1_busno,0x50);

  if (ret_code < 0) {
    return -ret_code;
  }

#endif /* RTEMS_BSP_I2C_EEPROM_DEVICE_NAME */

  /*
   * FIXME: register RTC driver, when available
   */
  return 0;
}
