/*
 *  This file contains a typical set of register access routines which may be
 *  used with the icm7170 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
 *    + registers are only byte-aligned (no address gaps)
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#ifndef _ICM7170_MULTIPLIER
#define _ICM7170_MULTIPLIER 1
#define _ICM7170_NAME(_X) _X
#define _ICM7170_TYPE uint8_t
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_ICM7170_TYPE *)((_base) + ((_reg) * _ICM7170_MULTIPLIER ))

/*
 *  ICM7170 Get Register Routine
 */

uint32_t   _ICM7170_NAME(icm7170_get_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum
)
{
  _ICM7170_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  ICM7170 Set Register Routine
 */

void  _ICM7170_NAME(icm7170_set_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
)
{
  _ICM7170_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
