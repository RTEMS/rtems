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
 *  _Thread_queue_Dequeue_fifo
 *
 *  This routine removes a thread from the specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *
 *  Output parameters:
 *    returns - thread dequeued or NULL
 *
 *  INTERRUPT LATENCY:
 *    check sync
 *    FIFO
 */

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

  switch ( the_thread_queue->sync_state ) {
    case THREAD_QUEUE_SYNCHRONIZED:
    case THREAD_QUEUE_SATISFIED:
      _ISR_Enable( level );
      return NULL;

    case THREAD_QUEUE_NOTHING_HAPPENED:
    case THREAD_QUEUE_TIMEOUT:
      the_thread_queue->sync_state = THREAD_QUEUE_SATISFIED;
      _ISR_Enable( level );
      return _Thread_Executing;
  }
  return NULL;                /* this is only to prevent warnings */
}

