/*
 *  Thread Queue Handler
 *
 *
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

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*
 *  _Thread_queue_Enqueue_with_handler
 *
 *  This routine blocks a thread, places it on a thread, and optionally
 *  starts a timeout timer.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    timeout          - interval to wait
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Thread_queue_Enqueue_with_handler(
  Thread_queue_Control         *the_thread_queue,
  Watchdog_Interval             timeout,
  Thread_queue_Timeout_callout  handler
)
{
  Thread_Control                   *the_thread;
  ISR_Level                         level;
  Thread_blocking_operation_States  sync_state;
  Thread_blocking_operation_States (*enqueue_p)(
    Thread_queue_Control *,
    Thread_Control *,
    ISR_Level *
  );

  the_thread = _Thread_Executing;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet )
    the_thread = _Thread_MP_Allocate_proxy( the_thread_queue->state );
  else
#endif
  /*
   *  Set the blocking state for this thread queue in the thread.
   */
  _Thread_Set_state( the_thread, the_thread_queue->state );

  /*
   *  If the thread wants to timeout, then schedule its timer.
   */
  if ( timeout ) {
    _Watchdog_Initialize(
       &the_thread->Timer,
       handler,
       the_thread->Object.id,
       NULL
    );

    _Watchdog_Insert_ticks( &the_thread->Timer, timeout );
  }

  /*
   *  Now enqueue the thread per the discipline for this thread queue.
   */
  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY )
    enqueue_p = _Thread_queue_Enqueue_priority;
  else /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
    enqueue_p = _Thread_queue_Enqueue_fifo;

  sync_state = (*enqueue_p)( the_thread_queue, the_thread, &level );
  if ( sync_state != THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED )
    _Thread_blocking_operation_Cancel( sync_state, the_thread, level );
}
