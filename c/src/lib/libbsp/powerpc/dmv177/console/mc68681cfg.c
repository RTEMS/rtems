/*
 *  This file contains the console driver chip level routines for the
 *  mc68681 chip.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (unsigned8 *)((_base) + ((_reg) * 8 ))

/* 
 *  Read_mc68681_register
 */

unsigned8 Read_mc68681_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  unsigned8 *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  Write_mc68681_register
 */

void  Write_mc68681_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  unsigned8 *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
