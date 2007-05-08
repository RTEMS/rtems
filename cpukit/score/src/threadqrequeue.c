/*
 *  Thread Queue Handler
 *
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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

/*PAGE
 *
 *  _Thread_queue_Requeue
 *
 *  This routine is invoked when a thread changes priority and should be
 *  moved to a different position on the thread queue.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to a thread control block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY: NONE
 */

void _Thread_queue_Requeue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  /* just in case the thread really wasn't blocked here */
  if ( !the_thread_queue )
    return;

  switch ( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      /* queueing by FIFO -- nothing to do */
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY: {
      Thread_queue_Control *tq = the_thread_queue;

      _Thread_queue_Enter_critical_section( tq );
        _Thread_queue_Extract_priority_helper( tq, the_thread, TRUE );
        _Thread_queue_Enqueue_priority( tq, the_thread );
      }
      break;
   }
}

