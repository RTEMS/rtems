/*
 *  This file contains a typical set of register access routines which may be
 *  used with the z85c30 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
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

#ifndef _Z85C30_MULTIPLIER
#define _Z85C30_MULTIPLIER 1
#define _Z85C30_NAME(_X) _X
#define _Z85C30_TYPE unsigned8
#endif

/*
 *  Z85C30 Get Register Routine
 */

unsigned8 _Z85C30_NAME(z85c30_get_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  _Z85C30_TYPE          *port;
  unsigned8              data;
  rtems_interrupt_level  level;

  port = (_Z85C30_TYPE *)ulCtrlPort;

  rtems_interrupt_disable(level);

    if(ucRegNum) {
      *port = ucRegNum;
    }
    data = *port;
  rtems_interrupt_enable(level);

  return data;
}

/*
 *  Z85C30 Set Register Routine
 */

void _Z85C30_NAME(z85c30_set_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  _Z85C30_TYPE          *port;
  rtems_interrupt_level  level;

  port = (_Z85C30_TYPE *)ulCtrlPort;

  rtems_interrupt_disable(level);
    if(ucRegNum) {
      *port = ucRegNum;
    }
    *port = ucData;
  rtems_interrupt_enable(level);
}
