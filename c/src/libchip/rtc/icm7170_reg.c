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
 *
 *  $Id$
 */

#include <rtems.h>

#ifndef _ICM7170_MULTIPLIER
#define _ICM7170_MULTIPLIER 1
#define _ICM7170_NAME(_X) _X
#define _ICM7170_TYPE unsigned8
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_ICM7170_TYPE *)((_base) + ((_reg) * _ICM7170_MULTIPLIER ))

/* 
 *  ICM7170 Get Register Routine
 */

unsigned32 _ICM7170_NAME(icm7170_get_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
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
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
)
{
  _ICM7170_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
