/*
 *  This file contains a typical set of register access routines which may be
 *  used with the mg5uart chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as unsigned32's
 *    + registers are only u32-aligned (no address gaps)
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>

#ifndef _MG5UART_MULTIPLIER
#define _MG5UART_MULTIPLIER 1
#define _MG5UART_NAME(_X) _X
#define _MG5UART_TYPE unsigned32
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_MG5UART_TYPE *)((_base) + ((_reg) * _MG5UART_MULTIPLIER ))

/* 
 *  MG5UART Get Register Routine
 */

unsigned8 _MG5UART_NAME(mg5uart_get_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  _MG5UART_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  MG5UART Set Register Routine
 */

void  _MG5UART_NAME(mg5uart_set_register)(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  _MG5UART_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
