/*  void Shm_Poll()
 *
 *  This routine polls to see if a packet has arrived.  If one
 *  has it informs the executive.  It is typically called from
 *  the clock tick interrupt service routine.
 *
 *  Input parameters:  NONE
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
 *  poll.c,v 1.2 1995/05/09 20:22:57 joel Exp
 */

#include <rtems.h>
#include <rtems/sysstate.h>
#include "shm.h"
#include "clockdrv.h"

void Shm_Poll()
{
  rtems_unsigned32 tmpfront;

  Clock_isr( 0 );             /* invoke standard clock ISR */


  /*
   * Check for msgs only if we are "up"
   * This avoids a race condition where we may get a clock
   * interrupt before MPCI has completed its init
   */
  
  if (_System_state_Is_up(_System_state_Get()))
  {
      /* enable_tracing(); */
      /* ticks += 1; */
      Shm_Lock( Shm_Local_receive_queue );
      tmpfront = Shm_Local_receive_queue->front;
      Shm_Unlock( Shm_Local_receive_queue );
      if ( Shm_Convert(tmpfront) != Shm_Locked_queue_End_of_list ) {
        rtems_multiprocessing_announce();
        Shm_Interrupt_count++;
      }
  }
}
