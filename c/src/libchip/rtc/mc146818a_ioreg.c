/*
 *  This file contains a typical set of register access routines which may be
 *  used with the MC146818A chip if accesses to the chip are as follows:
 *
 *    + registers are in I/O space
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
#include <bsp.h>

unsigned32 mc146818a_get_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  unsigned8   val;
  unsigned8   tmp;

  outport_byte( ulCtrlPort, ucRegNum );
  inport_byte( 0x84, tmp );   /* Hack a delay to give chip time to settle */
  inport_byte( ulCtrlPort+1, val );
  inport_byte( 0x84, tmp );   /* Hack a delay to give chip time to settle */
  return val;
}

void  mc146818a_set_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
)
{
  outport_byte( ulCtrlPort, ucRegNum );
  outport_byte( ulCtrlPort+1, (unsigned8)ucData );
}
