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

Scheduler_Void_or_thread _Scheduler_simple_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_simple_Context *context;
  Scheduler_Node           *node;
  bool                      prepend_it;

  if ( !_Thread_Is_ready( the_thread ) ) {
    /* Nothing to do */
    SCHEDULER_RETURN_VOID_OR_NULL;
  }

  context = _Scheduler_simple_Get_context( scheduler );
  node = _Scheduler_Thread_get_node( the_thread );
  _Scheduler_Node_get_priority( node, &prepend_it );

  _Scheduler_simple_Extract( scheduler, the_thread );

  if ( prepend_it ) {
    _Scheduler_simple_Insert_priority_lifo( &context->Ready, the_thread );
  } else {
    _Scheduler_simple_Insert_priority_fifo( &context->Ready, the_thread );
  }

  _Scheduler_simple_Schedule_body( scheduler, the_thread, false );

  SCHEDULER_RETURN_VOID_OR_NULL;
}
