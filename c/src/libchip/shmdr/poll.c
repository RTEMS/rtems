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
#include <rtems/score/sysstate.h>
#include <rtems/libio.h>

#include "shm_driver.h"

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
