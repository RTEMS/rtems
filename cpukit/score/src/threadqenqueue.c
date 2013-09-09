/**
 * @file
 *
 * @brief Thread Queue Enqueue
 * @ingroup ScoreThreadQ
 */

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

#include <rtems/score/threadqimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_queue_Enqueue_with_handler(
  Thread_queue_Control         *the_thread_queue,
  Thread_Control               *the_thread,
  Watchdog_Interval             timeout,
  Thread_queue_Timeout_callout  handler
)
{
  ISR_Level                         level;
  Thread_blocking_operation_States  sync_state;
  Thread_blocking_operation_States (*enqueue_p)(
    Thread_queue_Control *,
    Thread_Control *,
    ISR_Level *
  );

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
