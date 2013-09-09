/**
 * @file
 *
 * @brief Removes a Thread from a Thread  Queue
 *
 * @ingroup ScoreThreadQ
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

bool _Thread_queue_Extract_fifo(
  Thread_Control       *the_thread
)
{
  ISR_Level level;

  _ISR_Disable( level );

  if ( !_States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    _ISR_Enable( level );
    return false;
  }

  _Chain_Extract_unprotected( &the_thread->Object.Node );

  the_thread->Wait.queue = NULL;

  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  }

  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif

  return true;
}
