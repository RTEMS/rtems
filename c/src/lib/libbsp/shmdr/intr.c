/*  void Shm_Cause_interrupt( node )
 *
 *  This routine is the shared memory driver routine which
 *  generates interrupts to other CPUs.
 *
 *  It uses the information placed in the node status control
 *  block by each node.  For example, when used with the Motorola
 *  MVME136 board, the MPCSR is used.
 *
 *  Input parameters:
 *    node          - destination of this packet (0 = broadcast)
 *
 *  Output parameters: NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include "shm_driver.h"

void Shm_Cause_interrupt(
  rtems_unsigned32 node
)
{
  Shm_Interrupt_information *intr;
  rtems_unsigned8  *u8;
  rtems_unsigned16 *u16;
  rtems_unsigned32 *u32;
  rtems_unsigned32  value;

  intr = &Shm_Interrupt_table[node];
  value = intr->value;

  switch ( intr->length ) {
    case NO_INTERRUPT:
       break;
    case BYTE:
      u8   = (rtems_unsigned8 *)intr->address;
      *u8  = (rtems_unsigned8) value;
      break;
    case WORD:
      u16   = (rtems_unsigned16 *)intr->address;
      *u16  = (rtems_unsigned16) value;
      break;
    case LONG:
      u32   = (rtems_unsigned32 *)intr->address;
      *u32  = (rtems_unsigned32) value;
      break;
  }
}
