/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains the implementation of
 *   _Thread_queue_Extract().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2015, 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/threadimpl.h>

void _Thread_queue_Extract( Thread_Control *the_thread )
{
  Thread_queue_Context  queue_context;
  Thread_queue_Queue   *queue;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  queue = the_thread->Wait.queue;

  if ( queue != NULL ) {
    _Thread_Wait_remove_request( the_thread, &queue_context.Lock_context );
    _Thread_queue_Context_set_MP_callout(
      &queue_context,
      _Thread_queue_MP_callout_do_nothing
    );
#if defined(RTEMS_MULTIPROCESSING)
    _Thread_queue_MP_set_callout( the_thread, &queue_context );
#endif
    ( *the_thread->Wait.operations->extract )(
      queue,
      the_thread,
      &queue_context
    );
    _Thread_queue_Resume( queue, the_thread, &queue_context );
  } else {
    _Thread_Wait_release( the_thread, &queue_context );
  }
}
