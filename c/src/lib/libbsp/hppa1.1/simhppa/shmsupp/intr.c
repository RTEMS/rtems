/*  void Shm_Cause_interrupt_simhppa( node )
 *
 *  This routine is the shared memory driver routine which
 *  generates interrupts to other CPUs.
 *
 *  Input parameters:
 *    node          - destination of this packet (0 = broadcast)
 *
 *  Output parameters: NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "stdio.h"

#include <rtems.h>
#include "shm.h"

void Shm_Cause_interrupt_simhppa(
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
      fprintf(
        stderr,
        "Shm_Cause_interrupt_simhppa: Writes of unsigned8 not supported!!!\n"
      );
      rtems_shutdown_executive( 0 );
      break;
    case WORD:
      u16   = (rtems_unsigned16 *)intr->address;
      fprintf(
        stderr,
        "Shm_Cause_interrupt_simhppa: Writes of unsigned8 not supported!!!\n"
      );
      rtems_shutdown_executive( 0 );
      break;
    case LONG:
      u32   = (rtems_unsigned32 *)intr->address;
      HPPA_ASM_STWAS( value, 0, u32 );
      break;
  }
}
