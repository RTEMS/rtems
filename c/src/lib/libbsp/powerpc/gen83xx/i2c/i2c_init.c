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
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the low level MPC83xx I2C driver parameters  |
\*===============================================================*/


#include <mpc83xx/mpc83xx_i2cdrv.h>
#include <libchip/i2c-2b-eeprom.h>
#include <libchip/i2c-sc620.h>
#include <bsp/irq.h>
#include <bsp.h>

static void i2c1_probe(mpc83xx_i2c_softc_t *self)
{
#if MPC83XX_CHIP_TYPE != 8309
  if (((mpc83xx.clk.sccr >> (31-1)) & 0x03) > 0) {
    self->base_frq =
      (BSP_bus_frequency
       /((mpc83xx.clk.sccr >> (31-1)) & 0x03));
  }
#else /* MPC83XX_CHIP_TYPE != 8309 */
  self->base_frq = BSP_bus_frequency;
#endif /* MPC83XX_CHIP_TYPE != 8309 */
}

#ifndef MPC83XX_BOARD_BR_UID
static void i2c2_probe(mpc83xx_i2c_softc_t *self)
{
  self->base_frq = BSP_bus_frequency;
}
#endif /* MPC83XX_BOARD_BR_UID */

static mpc83xx_i2c_desc_t mpc83xx_i2c_bus_tbl[] = {
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
      .base_frq = 0, /* will be set during probe */
      .probe = i2c1_probe
    }
  }
#ifndef MPC83XX_BOARD_BR_UID
  /* second channel */
  , {
    { /* public fields */
      .ops = &mpc83xx_i2c_ops,
      .size = sizeof(mpc83xx_i2c_bus_tbl[0]),
    },
    { /* our private fields */
      .reg_ptr = &mpc83xx.i2c[1],
      .initialized = FALSE,
      .irq_number = BSP_IPIC_IRQ_I2C2,
      .base_frq = 0, /* will be set during probe */
      .probe = i2c2_probe
    }
  }
#endif /* MPC83XX_BOARD_BR_UID */
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
  char device_path[] = "/dev/i2c?";
  size_t n = RTEMS_ARRAY_SIZE(mpc83xx_i2c_bus_tbl);
  size_t i;
  int i2c_busno[n];

  /*
   * init I2C library (if not already done)
   */
  rtems_libi2c_initialize ();

  /*
   * init I2C buses
   */
  for (i = 0; i < n; ++i) {
    mpc83xx_i2c_desc_t *desc = &mpc83xx_i2c_bus_tbl[i];

    (*desc->softc.probe)(&desc->softc);
    device_path[sizeof(device_path) - 2] = (char) ('1' + i);
    i2c_busno[i] = rtems_libi2c_register_bus(device_path, &desc->bus_desc);
  }

#if defined(RTEMS_BSP_I2C_EEPROM_DEVICE_NAME)
  if (n > 0) {
    /*
     * register EEPROM to bus 1, Address 0x50
     */
    rtems_libi2c_register_drv(RTEMS_BSP_I2C_EEPROM_DEVICE_NAME,
                 i2c_2b_eeprom_driver_descriptor,
                 i2c_busno[0],0x50);
  }
#elif defined(MPC83XX_BOARD_BR_UID)
  if (n > 0) {
    rtems_libi2c_register_drv(
      "sc620",
      &i2c_sc620_driver,
      i2c_busno[0],
      0x70
    );
  }
#else

  /*
   * We have no i2c configuration for this variant but need to mark
   * i2c_busno as used.
   */
   (void) i2c_busno[0]; /* avoid set but not used warning */
#endif

  /*
   * FIXME: register RTC driver, when available
   */

  return RTEMS_SUCCESSFUL;
}
