/**
 * @file
 *
 * @brief Removes a Thread from the Simple Queue
 *
 * @ingroup RTEMSScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimpleimpl.h>

void _Scheduler_simple_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_simple_Context *context;
  unsigned int              new_priority;

  if ( !_Thread_Is_ready( the_thread ) ) {
    /* Nothing to do */
    return;
  }

  context = _Scheduler_simple_Get_context( scheduler );
  new_priority = (unsigned int ) _Scheduler_Node_get_priority( node );

  _Scheduler_simple_Extract( scheduler, the_thread, node );
  _Scheduler_simple_Insert( &context->Ready, the_thread, new_priority );
  _Scheduler_simple_Schedule_body( scheduler, the_thread, false );
}
