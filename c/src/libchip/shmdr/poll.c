/*
 *  This routine polls to see if a packet has arrived.  If one
 *  has it informs the executive. It uses a Classic API Timer
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/score/sysstate.h>
#include <rtems/libio.h>

#include <assert.h>

#include "shm_driver.h"

rtems_timer_service_routine Shm_Poll_TSR(
  rtems_id  id,
  void     *ignored_address
)
{
  uint32_t tmpfront;

  /*
   *  This should NEVER happen but just in case.
   */
  if (!_System_state_Is_up(_System_state_Get()))
    return;

  tmpfront = Shm_Local_receive_queue->front;
  if ( Shm_Convert(tmpfront) != Shm_Locked_queue_End_of_list ) {
    rtems_multiprocessing_announce();
    Shm_Interrupt_count++;
  }

  (void) rtems_timer_reset( id );
}

void Shm_install_timer(void)
{
  rtems_id          id;
  rtems_status_code status;

  status = rtems_timer_create( rtems_build_name( 'S', 'H', 'P', 'L' ), &id );
  assert( !status );

  status = rtems_timer_fire_after( id, 1, Shm_Poll_TSR, NULL );
  assert( !status );
}

