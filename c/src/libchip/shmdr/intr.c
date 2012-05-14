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
 */

#include <rtems.h>
#include "shm_driver.h"

void Shm_Cause_interrupt(
  uint32_t   node
)
{
  Shm_Interrupt_information *intr;
  uint8_t    *u8;
  uint16_t   *u16;
  uint32_t   *u32;
  uint32_t    value;

  intr = &Shm_Interrupt_table[node];
  value = intr->value;

  switch ( intr->length ) {
    case NO_INTERRUPT:
       break;
    case BYTE:
      u8   = (uint8_t*)intr->address;
      *u8  = (uint8_t) value;
      break;
    case WORD:
      u16   = (uint16_t*)intr->address;
      *u16  = (uint16_t) value;
      break;
    case LONG:
      u32   = (uint32_t*)intr->address;
      *u32  = (uint32_t) value;
      break;
  }
}
