/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Timer API for the lpc176x bsp.
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

#ifndef LIBBSP_ARM_LPC176X_TIMER_H
#define LIBBSP_ARM_LPC176X_TIMER_H

#include <bsp/timer-defs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief resets timer counter and stops it.
 *
 * @param tnumber the device to be reseted
 * @return RTEMS_SUCCESSFUL if the timer was reseted successfuly.
 */
rtems_status_code lpc176x_timer_reset( lpc176x_timer_number tnumber );

/**
 * @brief Sets mode of the timer (timer, counter rising, counter falling
 *        or counter both edges)
 *
 * @param tnumber: the device to be setted
 * @param mode: the desired mode
 * @return RTEMS_SUCCESSFUL if the timer's mode was setted successfuly.
 */
rtems_status_code lpc176x_timer_set_mode(
  lpc176x_timer_number tnumber,
  lpc176x_timer_mode   mode
);

/**
 * @brief Starts the timer counter
 *
 * @param tnumber: the device to be started
 * @return RTEMS_SUCCESSFUL if the timer's was started successfuly.
 */
rtems_status_code lpc176x_timer_start( lpc176x_timer_number tnumber );

/**
 * @brief true if timer is started.
 *
 * @param tnumber: the timer number to check.
 * @param is_started: TRUE if the timer is running.
 * @return RTEMS_SUCCESSFUL if the started timer check was successfuly.
 */
rtems_status_code lpc176x_timer_is_started(
  lpc176x_timer_number tnumber,
  bool                *is_started
);

/**
 * @brief sets the resolution in microseconds of the timer
 *
 * @param tnumber: the device to be modified.
 * @param resolution: how many microseconds will mean each timer
 *                    counter unit.
 * @return RTEMS_SUCCESSFUL if the timer resolution was setted successfuly.
 */
rtems_status_code lpc176x_timer_set_resolution(
  lpc176x_timer_number tnumber,
  lpc176x_microseconds resolution
);

/**
 * @brief Configures the timer match
 *
 * @param tnumber: the device to be modified
 * @param match_port: which port of this timer will be setted
 * @param function: what the timer should do when match: stop timer, clear,
 *                  and/or interrupt
 * @param match_value: the value that the timer should match.
 * @return RTEMS_SUCCESSFUL if the timer was configured successfuly.
 */
rtems_status_code lpc176x_timer_match_config(
  lpc176x_timer_number   tnumber,
  lpc176x_match_port     match_port,
  lpc176x_match_function function,
  uint32_t               match_value
);

/**
 * @brief Configures the capture ports
 *
 * @param tnumber: the device to be modified
 * @param capture_port: which port of this timer will be setted
 * @param function: At which edge/s will the capture work, and
 *                  if it will interrupt
 */
rtems_status_code lpc176x_timer_capture_config(
  lpc176x_timer_number     tnumber,
  lpc176x_capture_port     capture_port,
  lpc176x_capture_function function
);

/**
 * @brief Configures the external match ports
 *
 * @param tnumber: the device to be modified
 * @param match_port: which match for this timer
 * @param function: what should do when match: set, clear toggle or nothing
 */
rtems_status_code lpc176x_timer_external_match_config(
  lpc176x_timer_number       tnumber,
  lpc176x_match_port         match_port,
  lpc176x_ext_match_function function
);

/**
 * @brief Gets the captured value
 *
 * @param tnumber: the device to be modified
 * @param capnumber: which capture port for this timer
 * @return the captured value
 */
uint32_t lpc176x_timer_get_capvalue(
  lpc176x_timer_number tnumber,
  lpc176x_capture_port capnumber
);

/**
 * @brief Gets the timer value
 *
 * @param tnumber: the device
 * @return the timer value
 */
uint32_t lpc176x_timer_get_timer_value( lpc176x_timer_number tnumber );

/**
 * @brief Sets the timer value
 *
 * @param tnumber: the timer to modify.
 * @param timer_value the value to set.
 */
rtems_status_code lpc176x_timer_set_timer_value(
  lpc176x_timer_number tnumber,
  uint32_t             lpc176x_timer_value
);

/**
 * @brief Timer generic isroutine.
 *
 * @param timernumber the number of timer.
 */
void lpc176x_timer_isr( void *lpc176x_timer_number );

/**
 * @brief Initializes timer in timer mode and resets counter but
 *        without starting it, and without any capture or
 *        match function.
 *
 * @param tnumber which timer
 * @return RTEMS_SUCCESSFUL when everything ok.
 */
rtems_status_code lpc176x_timer_init( lpc176x_timer_number tnumber );

/**
 * @brief Initializes timer in timer mode and resets counter but
 *        without starting it, and without any capture or
 *        match function.
 *
 * @param tnumber which timer to init
 * @param vector the functions to be used by the isr.
 * @return RTEMS_SUCCESSFUL when everything ok.
 */
rtems_status_code lpc176x_timer_init_with_interrupt(
  lpc176x_timer_number            tnumber,
  const lpc176x_isr_funct_vector *vector
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_TIMER_H */