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
 *  _Thread_queue_First_fifo
 *
 *  This routines returns a pointer to the first thread on the
 *  specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *
 *  Output parameters:
 *    returns - first thread or NULL
 */

Thread_Control *_Thread_queue_First_fifo(
  Thread_queue_Control *the_thread_queue
)
{
  if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) )
    return (Thread_Control *) the_thread_queue->Queues.Fifo.first;

  return NULL;
}

