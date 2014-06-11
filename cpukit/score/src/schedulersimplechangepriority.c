/**
 * @file
 *
 * @brief Removes a Thread from the Simple Queue
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimpleimpl.h>

Scheduler_Void_or_thread _Scheduler_simple_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
)
{
  Scheduler_simple_Context *context =
    _Scheduler_simple_Get_context( scheduler );

  _Scheduler_simple_Extract( scheduler, the_thread );

  if ( prepend_it ) {
    _Scheduler_simple_Insert_priority_lifo( &context->Ready, the_thread );
  } else {
    _Scheduler_simple_Insert_priority_fifo( &context->Ready, the_thread );
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
