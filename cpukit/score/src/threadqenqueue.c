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
 *  _Thread_queue_Enqueue
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

void _Thread_queue_Enqueue(
  Thread_queue_Control *the_thread_queue,
  Watchdog_Interval     timeout
)
{
  Thread_Control *the_thread;

  the_thread = _Thread_Executing;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet )
    the_thread = _Thread_MP_Allocate_proxy( the_thread_queue->state );
  else
#endif
    _Thread_Set_state( the_thread, the_thread_queue->state );

  if ( timeout ) {
    _Watchdog_Initialize(
       &the_thread->Timer,
       _Thread_queue_Timeout,
       the_thread->Object.id,
       NULL
    );

    _Watchdog_Insert_ticks( &the_thread->Timer, timeout );
  }

  switch( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      _Thread_queue_Enqueue_fifo( the_thread_queue, the_thread, timeout );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      _Thread_queue_Enqueue_priority( the_thread_queue, the_thread, timeout );
      break;
  }
}

