/*
 *  This file contains a typical set of register access routines which may be
 *  used with the mc68681 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
 *    + registers are only byte-aligned (no address gaps)
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>

#ifndef _MC68681_MULTIPLIER
#define _MC68681_MULTIPLIER 1
#define _MC68681_NAME(_X) _X
#define _MC68681_TYPE unsigned8
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_MC68681_TYPE *)((_base) + ((_reg) * _MC68681_MULTIPLIER ))

/* 
 *  MC68681 Get Register Routine
 */

unsigned8 _MC68681_NAME(mc68681_get_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  _MC68681_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  MC68681 Set Register Routine
 */

void  _MC68681_NAME(mc68681_set_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  _MC68681_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
