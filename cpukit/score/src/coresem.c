/**
 *  @file
 *
 *  @brief Core Semaphore Initialize
 *  @ingroup ScoreSemaphore
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coresemimpl.h>

void _CORE_semaphore_Initialize(
  CORE_semaphore_Control     *the_semaphore,
  CORE_semaphore_Disciplines  discipline,
  uint32_t                    initial_value
)
{
  the_semaphore->count = initial_value;

  _Thread_queue_Initialize( &the_semaphore->Wait_queue );

  if ( discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY ) {
    the_semaphore->operations = &_Thread_queue_Operations_priority;
  } else {
    the_semaphore->operations = &_Thread_queue_Operations_FIFO;
  }
}

Thread_Control *_CORE_semaphore_Was_deleted(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  the_thread->Wait.return_code = CORE_SEMAPHORE_WAS_DELETED;

  return the_thread;
}

Thread_Control *_CORE_semaphore_Unsatisfied_nowait(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  the_thread->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;

  return the_thread;
}
