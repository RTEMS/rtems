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
 *  _Thread_queue_Extract
 *
 *  This routine removes a specific thread from the specified threadq,
 *  deletes any timeout, and unblocks the thread.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to a thread control block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY: NONE
 */

void _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  switch ( the_thread_queue->discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      _Thread_queue_Extract_fifo( the_thread_queue, the_thread );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      _Thread_queue_Extract_priority( the_thread_queue, the_thread );
      break;
   }
}

