/**
 *  @file
 *
 *  @brief Thread Queue First
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

Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control * (*first_p)(Thread_queue_Control *);

  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY )
      first_p = _Thread_queue_First_priority;
  else /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
      first_p = _Thread_queue_First_fifo;

  return (*first_p)( the_thread_queue );
}
