/**
 * @file pinmux.c
 *
 * @ingroup tms570
 *
 * @brief I/O Multiplexing Module (IOMM) basic support
 */

/*
 * Copyright (c) 2015 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/tms570.h>
#include <bsp/tms570-pinmux.h>

/**
 * @brief select desired function of pin/ball
 *
 * The function setups multiplexer to interconnect pin with
 * specified function/peripheral. Pin number is index into pinmux
 * entries array. Predefined values for pins are in a format
 * TMS570_BALL_<column><row> (for example TMS570_BALL_N19).
 * The multiplexer allows to interconnect one pin to multiple
 * signal sources/sinks in the theory but it is usually bad choice.
 * The function sets only specified function and clears all other
 * connections.
 *
 * @param[in] pin_num  pin/ball identifier (index into pinmux array)
 * @param[in] pin_fnc  function number 0 .. 7, if value TMS570_PIN_FNC_AUTO
 *                     is specified then pin function is extracted from
 *                     pin_num argument
 * @retval Void
 */
void
tms570_bsp_pin_set_function(int pin_num, int pin_fnc)
{
  unsigned int pin_shift;
  volatile uint32_t *pinmmrx;

  if ( pin_fnc == TMS570_PIN_FNC_AUTO ) {
    pin_fnc = (pin_num & TMS570_PIN_FNC_MASK) >> TMS570_PIN_FNC_SHIFT;
  }
  tms570_bsp_pin_to_pinmmrx(&pinmmrx, &pin_shift, pin_num);
  *pinmmrx = (*pinmmrx & ~(0xff << pin_shift)) | (1 << (pin_fnc + pin_shift));
}

/**
 * @brief clear connection between pin and specified peripherals/function
 *
 * This function switches off given connection and leaves rest
 * of multiplexer setup intact.
 *
 * @param[in] pin_num  pin/ball identifier (index into pinmux array)
 * @param[in] pin_fnc  function number 0 .. 7, if value TMS570_PIN_FNC_AUTO
 *                     is specified then pin function is extracted from
 *                     pin_num argument
 * @retval Void
 */
void
tms570_bsp_pin_clear_function(int pin_num, int pin_fnc)
{
  unsigned int pin_shift;
  volatile uint32_t *pinmmrx;

  if ( pin_fnc == TMS570_PIN_FNC_AUTO ) {
    pin_fnc = (pin_num & TMS570_PIN_FNC_MASK) >> TMS570_PIN_FNC_SHIFT;
  }
  tms570_bsp_pin_to_pinmmrx(&pinmmrx, &pin_shift, pin_num);
  *pinmmrx = *pinmmrx & ~(1 << (pin_fnc+pin_shift));
}
