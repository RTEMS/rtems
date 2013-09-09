/**
 *  @file
 *
 *  @brief Thread Queue Dequeue FIFO
 *  @ingroup ScoreThreadQ
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

Thread_Control *_Thread_queue_Dequeue_fifo(
  Thread_queue_Control *the_thread_queue
)
{
  ISR_Level              level;
  Thread_Control *the_thread;

  _ISR_Disable( level );
  if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) ) {

    the_thread = (Thread_Control *)
       _Chain_Get_first_unprotected( &the_thread_queue->Queues.Fifo );

    the_thread->Wait.queue = NULL;
    if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
      _ISR_Enable( level );
      _Thread_Unblock( the_thread );
    } else {
      _Watchdog_Deactivate( &the_thread->Timer );
      _ISR_Enable( level );
      (void) _Watchdog_Remove( &the_thread->Timer );
      _Thread_Unblock( the_thread );
    }

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      _Thread_MP_Free_proxy( the_thread );
#endif

    return the_thread;
  }

  _ISR_Enable( level );
  return NULL;
}
