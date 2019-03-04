/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief API of the Watchdog driver for the lpc176x bsp in RTEMS.
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

#ifndef LIBBSP_ARM_LPC176X_WATCHDOG_H
#define LIBBSP_ARM_LPC176X_WATCHDOG_H

#include <bsp/watchdog-defs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Checks if the watchdog was executed by software or not. Set when
 *        the watchdog timer times out, cleared by software.
 *
 * @return TRUE if the watchdog was executed.
 *         FALSE otherwise.
 */
bool lpc176x_been_reset_by_watchdog( void );

/**
 * @brief Resets the watchdog timer.
 */
void lpc176x_watchdog_reset( void );

/**
 * @brief Configures the watchdog's timer.
 *
 * @param tcount Timer's out value.
 * @return RTEMS_SUCCESSFUL if the watchdog was configured successfully.
 */
rtems_status_code lpc176x_watchdog_config( lpc176x_microseconds tcount );

/**
 * @brief Configures the timer watchdog using interrupt.
 *
 * @param tcount Timer's out value.
 * @param interrupt Interrupt to register.
 * @return RTEMS_SUCCESSFUL if the watchdog was configured successfully
 *                          with interrupts.
 */
rtems_status_code lpc176x_watchdog_config_with_interrupt(
  lpc176x_wd_isr_funct interrupt,
  lpc176x_microseconds tcount
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* LIBBSP_ARM_LPC176X_WATCHDOG_H */
