/**
 *  @file
 *
 *  @brief Thread Queue Initialize
 *  @ingroup ScoreThreadQ
 */

/*
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
#include <rtems/score/chainimpl.h>

void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Thread_queue_Disciplines      the_discipline,
  States_Control                state,
  uint32_t                      timeout_status
)
{
  the_thread_queue->state          = state;
  the_thread_queue->discipline     = the_discipline;
  the_thread_queue->timeout_status = timeout_status;
  the_thread_queue->sync_state     = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

  if ( the_discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY ) {
    uint32_t   index;

    for( index=0 ;
         index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
         index++)
      _Chain_Initialize_empty( &the_thread_queue->Queues.Priority[index] );
  } else { /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
    _Chain_Initialize_empty( &the_thread_queue->Queues.Fifo );
  }

}
