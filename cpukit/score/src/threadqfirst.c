/**
 *  @file
 *
 *  @brief Thread Queue First
 *  @ingroup ScoreThreadQ
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>

Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;

  _Thread_queue_Acquire( the_thread_queue, &lock_context );
  the_thread = _Thread_queue_First_locked( the_thread_queue );
  _Thread_queue_Release( the_thread_queue, &lock_context );

  return the_thread;
}
