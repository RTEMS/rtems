/*
 *  This file contains a typical set of register access routines which may be
 *  used with the mg5uart chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as uint32_t  's
 *    + registers are only u32-aligned (no address gaps)
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#ifndef _MG5UART_MULTIPLIER
#define _MG5UART_MULTIPLIER 1
#define _MG5UART_NAME(_X) _X
#define _MG5UART_TYPE uint32_t
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_MG5UART_TYPE *)((_base) + ((_reg) * _MG5UART_MULTIPLIER ))

/*
 *  MG5UART Get Register Routine
 */

uint8_t   _MG5UART_NAME(mg5uart_get_register)(
  uint32_t    ulCtrlPort,
  uint8_t     ucRegNum
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
  uint32_t    ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
)
{
  _MG5UART_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
