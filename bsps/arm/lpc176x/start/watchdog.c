/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Watchdog controller for the mbed lpc176x family boards.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <assert.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/watchdog.h>
#include <bsp/io.h>

inline bool lpc176x_been_reset_by_watchdog( void )
{
  return ( ( LPC176X_WDMOD & LPC176X_WWDT_MOD_WDTOF ) ==
           LPC176X_WWDT_MOD_WDTOF );
}

inline void lpc176x_watchdog_reset( void )
{
  LPC176X_WDFEED = LPC176X_WDFEED_CON;
  LPC176X_WDFEED = LPC176X_WDFEED_CFG;
}

/**
 * @brief Enables the watchdog module, sets wd clock and wd timer.
 *
 * @param  tcount Timer's out value.
 * @return RTEMS_SUCCESSFUL if the configuration was done successfully.
 */
static inline rtems_status_code enable_module_and_set_clocksel(
  const lpc176x_microseconds tcount )
{
  rtems_status_code status_code;

  /* Sets clock. */
  LPC176X_WDCLKSEL = LPC176X_WWDT_CLKSEL_WDSEL_PCLK;

  /* Enables the watchdog module.  */
  status_code = lpc176x_module_enable( LPC176X_MODULE_WD,
    LPC176X_MODULE_PCLK_DEFAULT );
  RTEMS_CHECK_SC( status_code, "Enabling the watchdog module." );

  /* Set the watchdog timer constant value. */
  LPC176X_WDTC = ( LPC176X_CCLK / LPC176X_WD_PRESCALER_DIVISOR ) * tcount;

  return status_code;
}

rtems_status_code lpc176x_watchdog_config( const lpc176x_microseconds tcount )
{
  rtems_status_code status_code = enable_module_and_set_clocksel( tcount );

  /* Setup the Watchdog timer operating mode in WDMOD register. */
  LPC176X_WDMOD = LPC176X_WWDT_MOD_WDEN | LPC176X_WWDT_MOD_WDRESET;

  /* Enable the Watchdog by writing 0xAA followed by 0x55 to the
      WDFEED register. */
  lpc176x_watchdog_reset();

  return status_code;
}

rtems_status_code lpc176x_watchdog_config_with_interrupt(
  const lpc176x_wd_isr_funct interrupt,
  const lpc176x_microseconds tcount
)
{
  rtems_status_code status_code = enable_module_and_set_clocksel( tcount );

  /* Setup the Watchdog timer operating mode in WDMOD register. */
  LPC176X_WDMOD = LPC176X_WWDT_MOD_WDEN | LPC176X_WWDT_MOD_WDINT;

  status_code = rtems_interrupt_handler_install(
    LPC176X_WD_INTERRUPT_VECTOR_NUMBER,
    "watchdog_interrupt",
    RTEMS_INTERRUPT_UNIQUE,
    interrupt,
    NULL );

  /* Enable the Watchdog by writing 0xAA followed by 0x55 to the
      WDFEED register. */
  lpc176x_watchdog_reset();

  return status_code;
}