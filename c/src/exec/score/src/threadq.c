/*
 *  Thread Queue Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
 *  _Thread_queue_Initialize
 *
 *  This routine initializes the specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue      - pointer to a threadq header
 *    the_class             - class of the object to which this belongs
 *    discipline            - queueing discipline
 *    state                 - state of waiting threads
 *    proxy_extract_callout - MP specific callout
 *    timeout_status        - return on a timeout
 *
 *  Output parameters: NONE
 */

void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Objects_Classes               the_class,
  Thread_queue_Disciplines      the_discipline,
  States_Control                state,
  Thread_queue_Extract_callout  proxy_extract_callout,
  unsigned32                    timeout_status
)
{
  unsigned32 index;

  _Thread_queue_Extract_table[ the_class ] = proxy_extract_callout;

  the_thread_queue->state          = state;
  the_thread_queue->discipline     = the_discipline;
  the_thread_queue->timeout_status = timeout_status;
  the_thread_queue->sync_state     = THREAD_QUEUE_SYNCHRONIZED;

  switch ( the_discipline ) {
    case THREAD_QUEUE_DISCIPLINE_FIFO:
      _Chain_Initialize_empty( &the_thread_queue->Queues.Fifo );
      break;
    case THREAD_QUEUE_DISCIPLINE_PRIORITY:
      for( index=0 ;
           index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
           index++)
        _Chain_Initialize_empty( &the_thread_queue->Queues.Priority[index] );
      break;
  }

}
