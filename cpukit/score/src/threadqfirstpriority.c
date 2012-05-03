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
 *  _Thread_queue_First_priority
 *
 *  This routines returns a pointer to the first thread on the
 *  specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to thread queue
 *
 *  Output parameters:
 *    returns - first thread or NULL
 */

Thread_Control *_Thread_queue_First_priority (
  Thread_queue_Control *the_thread_queue
)
{
  uint32_t   index;

  for( index=0 ;
       index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
       index++ ) {
    if ( !_Chain_Is_empty( &the_thread_queue->Queues.Priority[ index ] ) )
      return (Thread_Control *) _Chain_First(
        &the_thread_queue->Queues.Priority[ index ]
      );
  }
  return NULL;
}
