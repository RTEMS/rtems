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
 */

#include <rtems.h>
#include <bsp.h>

/*
 *  At this point, not all CPUs or BSPs have defined in/out port routines.
 */
#if defined(__i386__) || defined(__PPC__)
#if defined(inport_byte)
uint32_t mc146818a_get_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum
)
{
  uint8_t   val;
  uint8_t   tmp;

  outport_byte( ulCtrlPort, ucRegNum );
  inport_byte( 0x84, tmp );   /* Hack a delay to give chip time to settle */
  inport_byte( ulCtrlPort+1, val );
  inport_byte( 0x84, tmp );   /* Hack a delay to give chip time to settle */
  return val;
}

void  mc146818a_set_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum,
  uint32_t  ucData
)
{
  outport_byte( ulCtrlPort, ucRegNum );
  outport_byte( ulCtrlPort+1, (uint8_t)ucData );
}
#endif
#endif
