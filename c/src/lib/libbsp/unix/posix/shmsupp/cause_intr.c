/*  void Shm_interrupt_unix( node )
 *
 *  This routine is the shared memory driver routine which
 *  generates interrupts to other CPUs.
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

#include <bsp.h>
#include <shm_driver.h>
#include <sys/types.h>      /* pid_t */

void Shm_Cause_interrupt_unix(
  rtems_unsigned32 node
)
{
  Shm_Interrupt_information *intr;
  intr = &Shm_Interrupt_table[node];
 
  _CPU_SHM_Send_interrupt( (pid_t) intr->address, intr->value );
}
