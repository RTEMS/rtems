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
 *  $Id$
 */

#include <rtems.h>
#include <rtems/score/sysstate.h>
#include <rtems/libio.h>

#include "shm.h"

void Shm_Poll()
{
  rtems_unsigned32 tmpfront;
  rtems_libio_ioctl_args_t args;

  /* invoke clock isr */
  args.iop = 0;
  args.command = rtems_build_name('I', 'S', 'R', ' ');
  (void) rtems_io_control(rtems_clock_major, rtems_clock_minor, &args);

  /*
   * Check for msgs only if we are "up"
   * This avoids a race condition where we may get a clock
   * interrupt before MPCI has completed its init
   */
  
  if (_System_state_Is_up(_System_state_Get()))
  {
      tmpfront = Shm_Local_receive_queue->front;
      if ( Shm_Convert(tmpfront) != Shm_Locked_queue_End_of_list )
      {
          rtems_multiprocessing_announce();
          Shm_Interrupt_count++;
      }
  }
}
