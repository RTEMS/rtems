/*
 *  Thread Queue Handler
 *
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

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*PAGE
 *
 *  _Thread_queue_Enqueue_fifo
 *
 *  This routine blocks a thread, places it on a thread, and optionally
 *  starts a timeout timer.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    the_thread       - pointer to the thread to block
 *    timeout          - interval to wait
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Thread_queue_Enqueue_fifo (
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  Watchdog_Interval    timeout
)
{
  ISR_Level            level;
  Thread_queue_States  sync_state;

  _ISR_Disable( level );

  sync_state = the_thread_queue->sync_state;
  the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

  switch ( sync_state ) {
    case THREAD_QUEUE_SYNCHRONIZED: 
      /*
       *  This should never happen.  It indicates that someone did not
       *  enter a thread queue critical section.
       */
      break;

    case THREAD_QUEUE_NOTHING_HAPPENED: 
      _Chain_Append_unprotected(
        &the_thread_queue->Queues.Fifo,
        &the_thread->Object.Node
      );
      _ISR_Enable( level );
      return;

    case THREAD_QUEUE_TIMEOUT: 
      the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;
      _ISR_Enable( level );
      break;

    case THREAD_QUEUE_SATISFIED:
      if ( _Watchdog_Is_active( &the_thread->Timer ) ) {
        _Watchdog_Deactivate( &the_thread->Timer );
        _ISR_Enable( level );
        (void) _Watchdog_Remove( &the_thread->Timer );
      } else
        _ISR_Enable( level );
      break;
  }

  /*
   *  Global objects with thread queue's should not be operated on from an
   *  ISR.  But the sync code still must allow short timeouts to be processed
   *  correctly.
   */

  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif

}

