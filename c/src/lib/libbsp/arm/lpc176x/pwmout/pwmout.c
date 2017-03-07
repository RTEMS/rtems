/**
 * @file pwmout.c
 *
 * @ingroup lpc176x
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

#include <rtems/status-checks.h>
#include <bsp/pwmout.h>
#include <bsp/pwmout-defs.h>

/**
 * @brief The low level device.
 */
static lpc176x_pwm_device *const pwm_device =
  (lpc176x_pwm_device *) PWM1_BASE_ADDR;

/**
 * @brief The possible output pins for each PWM output.
 */
static const lpc176x_pwm_pin pwm_pins[ PWM_OUTPUT_NUMBER ][ PWM_NUMBER_OF_PINS
] =
{
  { { 50u, LPC176X_PIN_FUNCTION_10 }, { 64u, LPC176X_PIN_FUNCTION_01 } },
  { { 52u, LPC176X_PIN_FUNCTION_10 }, { 65u, LPC176X_PIN_FUNCTION_01 } },
  { { 53u, LPC176X_PIN_FUNCTION_10 }, { 66u, LPC176X_PIN_FUNCTION_01 } },
  { { 55u, LPC176X_PIN_FUNCTION_10 }, { 67u, LPC176X_PIN_FUNCTION_01 } },
  { { 56u, LPC176X_PIN_FUNCTION_10 }, { 68u, LPC176X_PIN_FUNCTION_01 } },
  { { 58u, LPC176X_PIN_FUNCTION_10 }, { 69u, LPC176X_PIN_FUNCTION_01 } },
};

/**
 * @brief The pointers to the low level match registers for each PWM output.
 */
static volatile uint32_t *const pwm_match[ PWM_OUTPUT_NUMBER ] = {
  &PWM1MR1,
  &PWM1MR2,
  &PWM1MR3,
  &PWM1MR4,
  &PWM1MR5,
  &PWM1MR6
};

/**
 * @brief Checks if a pin number is valid for the given PWM,
 *  and sets the corresponding pin function for that pin.
 *
 * @param pin_number The pin number to search.
 * @param pwm In which PWM search for the pin number.
 * @param pin_function If the pin number is found, here we return
 *  the pin function for that pin number.
 * @return True if found, false otherwise.
 */
static inline bool is_found_in_this_pwm(
  const lpc176x_pin_number       pin_number,
  const lpc176x_pwm_number    pwm,
  lpc176x_pin_function *const pin_function
)
{
  lpc176x_pwm_pin_number pnumber = PWM_FIRST_PIN;
  bool                   found = false;

  while (!found && ( pnumber < PWM_NUMBER_OF_PINS ))
  {
    if ( pwm_pins[ pwm ][ pnumber ].pin_number == pin_number ) {
      found = true;
      *pin_function = pwm_pins[ pwm ][ pnumber ].pin_function;
    }/*else implies that the pin number was not found. Keep looking.*/
    ++pnumber;
  }
  return found;
}

/**
 * @brief Checks if a pin number is valid for any PWM,
 *  and sets the corresponding pin function for that pin.
 *
 * @param pin_number The pin number to search.
 * @param pwm If is found here we return in which PWM was found.
 * @param pin_function If the pin number is found the pin function
 *  for this pin number one will be returned.
 * @return True if found, false otherwise.
 */
static bool is_valid_pin_number(
  const lpc176x_pin_number       pin_number,
  lpc176x_pwm_number *const   pwm,
  lpc176x_pin_function *const pin_function
)
{
  bool               found = false;
  lpc176x_pwm_number pwm_local = PWMO_1;
  while(!found && ( pwm_local < PWM_OUTPUT_NUMBER ))
  {
    if ( is_found_in_this_pwm( pin_number, pwm_local, pin_function ) ) {
      *pwm = pwm_local;
      found = true;
    } /*else implies that the pin number was not found. Keep looking.*/
    ++pwm_local;
  }

  return found;
}

/**
 * @brief Sets the period for the given PWM.
 *
 * @param pwm The PWM output in which the period will be set.
 * @param period The period to set.
 */
static void set_period(
  const lpc176x_pwm_number   pwm,
  const lpc176x_microseconds period
)
{
  pwm_device->TCR = PWM_TCR_RESET;
  pwm_device->MR0 = period * PWM_PRESCALER_USECOND;
  pwm_device->LER |= PWM_LER_LATCH_MATCH_0;
  pwm_device->TCR = PWM_TCR_PWM | PWM_TCR_ENABLE;
}

/**
 * @brief Sets the pulsewidth for the given PWM.
 *
 * @param pwm The PWM output in which the pulsewidth will be set.
 * @param pwidth The pulse width to set.
 */
static void set_pulsewidth(
  const lpc176x_pwm_number pwm,
  lpc176x_microseconds     pwidth
)
{
  pwidth *= PWM_PRESCALER_USECOND;

  if ( pwm_device->MR0 == pwidth ) {
    ++pwidth;
  } /* Not the same as the period, do nothing.*/

  *( pwm_match[ pwm ] ) = pwidth;
  pwm_device->LER |= PWM_LER_LATCH( pwm );
}

rtems_status_code pwm_init( const lpc176x_pin_number pin_number )
{
  rtems_status_code    sc = RTEMS_INVALID_NUMBER;
  lpc176x_pin_function pin_function;
  lpc176x_pwm_number   pwm;

  if ( is_valid_pin_number( pin_number, &pwm, &pin_function ) ) {
    sc = lpc176x_module_enable( LPC176X_MODULE_PWM_1,
      LPC176X_MODULE_PCLK_DEFAULT );
    RTEMS_CHECK_SC( sc, "enable pwm module" );

    pwm_device->PR = 0;
    pwm_device->MCR = PWM_MCR_RESET_ON_MATCH0;
    pwm_device->PCR |= PWM_PCR_ENABLE_PWM( pwm );

    set_period( pwm, PWM_DEFAULT_PERIOD );
    set_pulsewidth( pwm, PWM_DEFAULT_PULSEWIDTH );

    lpc176x_pin_select( pin_number, pin_function );
  } /* else implies that the pin number is not valid.
     So, a RTEMS_INVALID_NUMBER will be returned.*/

  return sc;
}

rtems_status_code pwm_period(
  const lpc176x_pin_number    pin_number,
  const lpc176x_microseconds period
)
{
  rtems_status_code    sc = RTEMS_INVALID_NUMBER;
  lpc176x_pin_function pin_function;
  lpc176x_pwm_number   pwm;

  if ( is_valid_pin_number( pin_number, &pwm, &pin_function ) ) {
    sc = RTEMS_SUCCESSFUL;
    set_period( pwm, period );
  } /* else implies that the pin number is not valid.
     So, a RTEMS_INVALID_NUMBER will be returned.*/

  return sc;
}

rtems_status_code pwm_pulsewidth(
  const lpc176x_pin_number    pin_number,
  const lpc176x_microseconds pwidth
)
{
  rtems_status_code    sc = RTEMS_INVALID_NUMBER;
  lpc176x_pin_function pin_function;
  lpc176x_pwm_number   pwm;

  if ( is_valid_pin_number( pin_number, &pwm, &pin_function ) ) {
    sc = RTEMS_SUCCESSFUL;
    set_pulsewidth( pwm, pwidth );
  } /* Else wrong pin_number return RTEMS_INVALID_NUMBER*/

  return sc;
}
