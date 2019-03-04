/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief PWM-Out controller for the mbed lpc1768 board.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LPC176X_PWMOUT_H
#define LPC176X_PWMOUT_H

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/lpc176x.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Initializes the PWM output device and sets an initial period
 *  of 20000us.
 *
 * @param pin_number The PWM output pin.
 * @return RTEMS_SUCCESSFULL  if the PWM initialization was OK,
 *  RTEMS_INVALID_NUMBER for a wrong parameter.
 */
rtems_status_code pwm_init( const lpc176x_pin_number pin_number );

/**
 * @brief Sets a period for the PWM output. (Note that this changes the period
 * for all the PWM outputs.)
 *
 * @param pin_number The pin whose period we want to change.
 * @param period The desired period in microseconds.
 *
 * @return RTEMS_SUCCESSFULL if the period's setting was OK,
 *  RTEMS_INVALID_NUMBER for a bad parameter.
 */
rtems_status_code pwm_period(
  const lpc176x_pin_number    pin_number,
  const lpc176x_microseconds period
);

/**
 * @brief Sets a pulsewidth for the PWM output.
 *
 * @param pin_number The pin whose pulsewidth we want to change.
 * @param pwidth The desired pulsewidth in microseconds.
 *
 * @return RTEMS_SUCCESSFULL if the pulsewidth's setting was OK,
 *  RTEMS_INVALID_NUMBER for a wrong parameter.
 */
rtems_status_code pwm_pulsewidth(
  const lpc176x_pin_number    pin_number,
  const lpc176x_microseconds pwidth
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
